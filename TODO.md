# Pevi Editor Development TODO

## Current Priority Issues

### High Priority
- [ ] **Fix observer registration error**: ECS complete example crashes with "observer must have at least one term" error
  - Location: `examples/ecs_complete/systems/observers.c` 
  - Issue: Observer created with no terms during RegisterObservers() call
  - Need to investigate component availability for `ecs_id(Selected)` and `ecs_id(FileReference)`

### Medium Priority  
- [ ] **Test all ECS examples**: Verify other examples still work after component registration fixes
- [ ] **Debug simple vs complete example differences**: Understand why simple example works but complete doesn't
- [ ] **Improve error handling**: Add proper error checking in component registration

### Low Priority
- [ ] **Complete ECS implementation**: Continue building out the 3D spatial editor features
- [ ] **Add text rendering system**: Implement actual 3D text rendering for phantoms
- [ ] **File I/O integration**: Real file loading and watching functionality

## Completed Tasks
- [x] **Fixed ECS compilation issues**: Resolved component registration conflicts
- [x] **Fixed ecs_set syntax**: Proper handling of Position and complex struct components  
- [x] **Added missing dependencies**: glfw library linking and Includes relationship
- [x] **Build system working**: All examples compile successfully

## Notes
- The simple ECS example (`spatial_editor_simple`) works perfectly
- Main compilation issues were resolved but runtime observer error remains
- Components are properly declared but may need different registration order
