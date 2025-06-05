#include <joltc.h>
#include <stdio.h>
static void TraceImpl(const char *message) {
  // Print to the TTY
  printf("%s\n", message);
}

int main() {
  if (!JPH_Init())
    return 1;

  JPH_SetTraceHandler(TraceImpl);
  // JPH_SetAssertFailureHandler(JPH_AssertFailureFunc handler);

  JPH_JobSystem *jobSystem = JPH_JobSystemThreadPool_Create(NULL);

  // We use only 2 layers: one for non-moving objects and one for moving objects
  JPH_ObjectLayerPairFilter *objectLayerPairFilterTable =
      JPH_ObjectLayerPairFilterTable_Create(2);
  JPH_ObjectLayerPairFilterTable_EnableCollision(objectLayerPairFilterTable, 0,
                                                 1);
  JPH_ObjectLayerPairFilterTable_EnableCollision(objectLayerPairFilterTable, 1,
                                                 0);

  // We use a 1-to-1 mapping between object layers and broadphase layers
  JPH_BroadPhaseLayerInterface *broadPhaseLayerInterfaceTable =
      JPH_BroadPhaseLayerInterfaceTable_Create(2, 2);
  JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(
      broadPhaseLayerInterfaceTable, 0, 0);
  JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(
      broadPhaseLayerInterfaceTable, 1, 1);

  JPH_ObjectVsBroadPhaseLayerFilter *objectVsBroadPhaseLayerFilter =
      JPH_ObjectVsBroadPhaseLayerFilterTable_Create(
          broadPhaseLayerInterfaceTable, 2, objectLayerPairFilterTable, 2);

  JPH_PhysicsSystemSettings settings = {0};
  settings.maxBodies = 65536;
  settings.numBodyMutexes = 0;
  settings.maxBodyPairs = 65536;
  settings.maxContactConstraints = 65536;
  settings.broadPhaseLayerInterface = broadPhaseLayerInterfaceTable;
  settings.objectLayerPairFilter = objectLayerPairFilterTable;
  settings.objectVsBroadPhaseLayerFilter = objectVsBroadPhaseLayerFilter;
  JPH_PhysicsSystem *system = JPH_PhysicsSystem_Create(&settings);
  JPH_BodyInterface *bodyInterface = JPH_PhysicsSystem_GetBodyInterface(system);

  JPH_BodyID floorId = {0};
  {
    // Next we can create a rigid body to serve as the floor, we make a large
    // box Create the settings for the collision volume (the shape). Note that
    // for simple shapes (like boxes) you can also directly construct a
    // BoxShape.
    JPH_Vec3 boxHalfExtents = {100.0f, 1.0f, 100.0f};
    JPH_BoxShape *floorShape =
        JPH_BoxShape_Create(&boxHalfExtents, JPH_DEFAULT_CONVEX_RADIUS);

    JPH_Vec3 floorPosition = {0.0f, -1.0f, 0.0f};
    JPH_BodyCreationSettings *floorSettings = JPH_BodyCreationSettings_Create3(
        (const JPH_Shape *)floorShape, &floorPosition,
        NULL, // Identity,
        JPH_MotionType_Static, 0);

    // Create the actual rigid body
    floorId = JPH_BodyInterface_CreateAndAddBody(bodyInterface, floorSettings,
                                                 JPH_Activation_DontActivate);
    JPH_BodyCreationSettings_Destroy(floorSettings);
  }
  // Sphere
  JPH_BodyID sphereId = {0};
  {
    JPH_SphereShape *sphereShape = JPH_SphereShape_Create(50.0f);
    JPH_Vec3 spherePosition = {0.0f, 2.0f, 0.0f};
    JPH_BodyCreationSettings *sphereSettings = JPH_BodyCreationSettings_Create3(
        (const JPH_Shape *)sphereShape, &spherePosition,
        NULL, // Identity,
        JPH_MotionType_Dynamic, 1);

    sphereId = JPH_BodyInterface_CreateAndAddBody(bodyInterface, sphereSettings,
                                                  JPH_Activation_Activate);
    JPH_BodyCreationSettings_Destroy(sphereSettings);
  }

  // Now you can interact with the dynamic body, in this case we're going to
  // give it a velocity. (note that if we had used CreateBody then we could have
  // set the velocity straight on the body before adding it to the physics
  // system)
  JPH_Vec3 sphereLinearVelocity = {0.0f, -5.0f, 0.0f};
  JPH_BodyInterface_SetLinearVelocity(bodyInterface, sphereId,
                                      &sphereLinearVelocity);

  {
    static float cCharacterHeightStanding = 1.35f;
    static float cCharacterRadiusStanding = 0.3f;
    static float cCharacterHeightCrouching = 0.8f;
    static float cCharacterRadiusCrouching = 0.3f;
    static float cInnerShapeFraction = 0.9f;

    JPH_CapsuleShape *capsuleShape = JPH_CapsuleShape_Create(
        0.5f * cCharacterHeightStanding, cCharacterRadiusStanding);
    JPH_Vec3 position = {
        0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0};
    JPH_RotatedTranslatedShape *mStandingShape =
        JPH_RotatedTranslatedShape_Create(&position, NULL,
                                          (JPH_Shape *)capsuleShape);

    JPH_CharacterVirtualSettings characterSettings = {0};
    JPH_CharacterVirtualSettings_Init(&characterSettings);
    characterSettings.base.shape = (const JPH_Shape *)mStandingShape;
    JPH_Plane supportingVolume = {
        {0, 1, 0}, -cCharacterRadiusStanding}; // Accept contacts that touch the
                                               // lower sphere of the capsule
    characterSettings.base.supportingVolume = supportingVolume;
    static const JPH_RVec3 characterVirtualPosition = {-5.0f, 0, 3.0f};

    JPH_CharacterVirtual *mAnimatedCharacterVirtual =
        JPH_CharacterVirtual_Create(&characterSettings,
                                    &characterVirtualPosition, NULL, 0, system);
  }

  JPH_SixDOFConstraintSettings jointSettings;
  JPH_SixDOFConstraintSettings_Init(&jointSettings);

  // We simulate the physics world in discrete time steps. 60 Hz is a good rate
  // to update the physics system.
  const float cDeltaTime = 1.0f / 60.0f;

  // Optional step: Before starting the physics simulation you can optimize the
  // broad phase. This improves collision detection performance (it's pointless
  // here because we only have 2 bodies). You should definitely not call this
  // every frame or when e.g. streaming in a new level section as it is an
  // expensive operation. Instead insert all new objects in batches instead of 1
  // at a time to keep the broad phase efficient.
  JPH_PhysicsSystem_OptimizeBroadPhase(system);

  // Now we're ready to simulate the body, keep simulating until it goes to
  // sleep
  uint32_t step = 0;
  while (JPH_BodyInterface_IsActive(bodyInterface, sphereId)) {
    // Next step
    ++step;

    // Output current position and velocity of the sphere
    JPH_RVec3 position;
    JPH_Vec3 velocity;

    JPH_BodyInterface_GetCenterOfMassPosition(bodyInterface, sphereId,
                                              &position);
    JPH_BodyInterface_GetLinearVelocity(bodyInterface, sphereId, &velocity);
    printf("Step %d: Position = (%f, %f, %f), Velocity = (%f, %f, %f)\n",
           step, position.x, position.y, position.z, velocity.x, velocity.y,
           velocity.z);

    // If you take larger steps than 1 / 60th of a second you need to do
    // multiple collision steps in order to keep the simulation stable. Do 1
    // collision step per 1 / 60th of a second (round up).
    const int cCollisionSteps = 1;

    // Step the world
    JPH_PhysicsSystem_Update(system, cDeltaTime, cCollisionSteps, jobSystem);
  }

  // Remove the destroy sphere from the physics system. Note that the sphere
  // itself keeps all of its state and can be re-added at any time.
  JPH_BodyInterface_RemoveAndDestroyBody(bodyInterface, sphereId);

  // Remove and destroy the floor
  JPH_BodyInterface_RemoveAndDestroyBody(bodyInterface, floorId);

  JPH_JobSystem_Destroy(jobSystem);

  JPH_PhysicsSystem_Destroy(system);
  JPH_Shutdown();
  return 0;
}
