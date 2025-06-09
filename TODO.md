# Pevi Editor Development TODO

## Current Priority Issues
✅ **COMPLETED**: Fixed observer registration error that was crashing the ECS complete example

## Recently Completed Tasks
- ✅ **Fixed observer registration syntax**: Replaced compound literal with explicit descriptor initialization 
- ✅ **Fixed component registration API**: Used ECS_COMPONENT_DEFINE instead of ECS_COMPONENT
- ✅ **Fixed tag registration API**: Used ECS_TAG_DEFINE instead of ECS_TAG
- ✅ **Fixed component hook conflicts**: Replaced ecs_component() with ecs_set_hooks()
- ✅ **Fixed entity naming conflict**: Renamed "EditorState" entity to "Editor"
- ✅ **Verified all ECS examples work**: Both simple and complete examples now run successfully

## Medium Priority  
- [ ] **Complete camera controller implementation**: Add smooth camera transitions and constraints
- [ ] **Implement 3D text rendering system**: Actual 3D text rendering for phantoms in space
- [ ] **Add input handling systems**: Mouse picking, keyboard shortcuts, mode switching
- [ ] **Improve selection system**: Visual feedback, multi-selection, selection persistence

## Low Priority
- [ ] **File I/O integration**: Real file loading, watching, and hot reloading functionality
- [ ] **Command system**: Text-based command interface for editor operations
- [ ] **Performance optimization**: Spatial partitioning, LOD systems, frustum culling
- [ ] **Visual improvements**: Better materials, lighting, particle effects for code elements

## Technical Debt
- [ ] **Add comprehensive error handling**: Proper error checking in all ECS operations
- [ ] **Create unit tests**: Test component registration, observer behavior, file loading
- [ ] **Documentation**: Complete API documentation for all ECS components and systems
- [ ] **Code review**: Ensure all ECS patterns follow Flecs best practices

## Future Features
- [ ] **Multi-file project support**: Handle large codebases with multiple files and directories
- [ ] **Code dependency visualization**: Show includes, function calls, variable references as 3D connections
- [ ] **Collaborative editing**: Multi-user editing in the same 3D space
- [ ] **Plugin system**: Support for language-specific analyzers and renderers

## Notes
- The core ECS architecture is now solid and ready for feature development
- Observer system provides reactive programming for file changes and selection events
- Component registration patterns are established and documented
- Ready to focus on user-facing features and 3D editor functionality
