# Pevi Editor - Project Management Overview

## Project Status: NEEDS TESTING ⚠️
*Last Updated: June 27, 2025*

### **Current State - Requires Manual Testing:**
- 🔧 **Build System**: Compiles successfully, executables exist
- ❓ **Core Application**: Unknown if main editor functions properly
- ❓ **ECS Examples**: Unknown if spatial editors work correctly
- ❓ **Component Integration**: Unknown if all systems integrate
- ❓ **3D Rendering**: Unknown if graphics/text render properly
- ❓ **Input Handling**: Unknown if navigation/controls work

### **What We're Building:**
Pevi is a **3D spatial code editor** where code appears as floating "phantoms" (3D objects) arranged by **mnemonic sense** - meaning code is positioned in 3D space based on how developers naturally think about and remember relationships (call flow, data flow, hierarchy).

**Core Concept**: A single file is edited through multiple phantoms, each showing functions, classes, or logical blocks positioned intuitively in 3D space for better understanding and navigation.

---

## **Development Setup Status**

### **CRITICAL ISSUES TO FIX:**
1. **🔥 Git Repository Chaos**
   - 27 unpushed commits sitting locally
   - Build artifacts (*.o files, CMakeFiles) being tracked in git
   - Working directory is dirty with build files

2. **🔥 Missing Team Workflow**
   - No GitHub Issues for task management
   - No Pull Request workflow
   - No branch strategy for collaborative development

3. **🔥 Documentation Redundancy**
   - Duplicate content in CLAUDE.md, TODO.md, ARCHITECTURE.md
   - No single source of truth for project state

---

## **Technology Stack**
- **Language**: C11
- **Graphics**: Raylib (3D rendering, input, windowing)
- **ECS**: Flecs (entity-component-system architecture)
- **Physics**: JoltC (planned)
- **UI**: ImGui (planned)
- **Build**: CMake with vcpkg for dependencies
- **Platform**: macOS ARM64 (primary), cross-platform goal

---

## **Current Architecture**
```
Application Layer: Lifecycle, configuration, system coordination
    ↓
Core Layer: Component framework, event system, spatial management
    ↓
Domain Layer: Text engine, phantom system, command processor
    ↓  
Services Layer: Renderer (Raylib), input, file I/O, UI
```

**Key Components:**
- **Phantoms**: 3D representations of code blocks positioned by mnemonic sense
- **Camera System**: 3D navigation with smooth controls
- **Text Engine**: Buffer management with 3D text rendering
- **Input System**: Mode-based input (navigation/edit/command)

---

## **Next Priority Actions**

### **Phase 1: URGENT - Manual Testing (CURRENT)**
**GitHub Issues Created:**
- Issue #2: Test core application startup and basic functionality
- Issue #3: Test ECS spatial editor examples
- Issue #4: Test all standalone examples and integrations  
- Issue #5: Test build system from clean environment
- Issue #6: Clean git repository and sync (BLOCKER)

**CRITICAL**: We need to complete manual testing before any new development. Current claims of functionality are unverified.

### **Phase 2: Fix Critical Issues**
Based on testing results, create specific bug fix issues for:
- Application crashes or startup failures
- Graphics/rendering problems
- Input handling bugs
- Build system issues
- Integration failures

### **Phase 3: Establish Development Workflow** 
1. Complete git repository cleanup (Issue #6)
2. Set up branch protection requiring PR reviews
3. Create issue templates and development guidelines
4. Establish CI/CD for automated testing

---

## **Active Development Areas**

### **Working and Ready for Feature Development:**
- Core ECS with proper component registration patterns
- 3D text rendering with billboard display
- Camera navigation with smooth controls
- Input handling for 3D interaction
- File structure with proper layered architecture

### **Next Features to Implement:**
1. **Edit Mode Input**: Text editing within phantoms
2. **Command System**: Basic command interface (:save, :load, etc.)
3. **File I/O Integration**: Load real code files into phantoms
4. **Selection System**: Visual feedback for selected phantoms
5. **Phantom Positioning**: Implement mnemonic sense algorithms

### **Technical Debt to Address:**
- Add comprehensive error handling
- Create unit tests for ECS components
- Performance optimization for large files
- Memory management improvements

---

## **Team Coordination Rules**

### **Code Flow (MUST FOLLOW):**
- **ALL code changes MUST go through Pull Requests**
- **NO direct commits to master branch**
- **Require at least 1 review before merge**
- **All PRs must have linked GitHub Issue**

### **Issue Management:**
- Break big features into small, actionable GitHub Issues
- Assign issues to prevent developers working on same code
- Close completed issues immediately
- Delete/close stale or duplicate issues weekly

### **Branch Strategy:**
- `master`: Production-ready code only
- `develop`: Integration branch for features
- `feature/issue-123-description`: Feature branches linked to issues
- `bugfix/issue-456-description`: Bug fix branches

---

## **Development Commands**

### **Build and Run:**
```bash
# Full clean build
rm -rf build && mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../vcpkg_installed/arm64-osx/share/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build .

# Run main editor
./build/editor

# Run working examples
./build/examples/ecs_complete/spatial_editor
./build/examples/ecs_complete/spatial_editor_simple
```

### **Git Workflow:**
```bash
# Create feature branch
git checkout -b feature/issue-123-phantom-selection

# Regular commits during development
git add . && git commit -m "Implement phantom selection highlighting"

# Push and create PR
git push -u origin feature/issue-123-phantom-selection
# Then create PR via GitHub UI
```

---

## **File Structure**
```
├── src/                    # Main editor source
├── include/pevi/          # Public headers
├── examples/              # Working examples and demos
├── docs/                  # Architecture and technical docs
├── tests/                 # Unit tests (to be created)
├── assets/                # Fonts, shaders, resources
├── build/                 # Build output (git-ignored)
├── vcpkg_installed/       # Dependencies (git-ignored)
└── third_party/           # External libraries
```

**Key Files:**
- `src/main.c`: Entry point
- `examples/ecs_complete/`: Working ECS implementation
- `MANAGER.md`: This project management file
- `docs/ARCHITECTURE.md`: Technical architecture
- `TODO.md`: Legacy todo (to be converted to GitHub Issues)

---

## **Naming Conventions**

### **Functions:** `object_action` or `object_subobject_action`
```c
phantom_create()              // Basic actions
phantom_text_content_set()    // Subobject actions
camera_target_phantom_set()   // Complex operations
```

### **Structs:** PascalCase types, snake_case instances
```c
typedef struct { float x, y, z; } Position;  // Type
Position phantom_position;                   // Instance
```

### **Files:** snake_case.h/.c in appropriate directories
```
include/pevi/phantom/text_content.h
src/phantom/text_content.c
```

---

## **Success Metrics**
- [ ] Clean git repository with proper .gitignore
- [ ] All development via GitHub Issues + PRs
- [ ] No build artifacts in version control
- [ ] Team can work simultaneously without conflicts
- [ ] PRs reviewed and merged within 24 hours
- [ ] No stale issues older than 2 weeks
- [ ] Core functionality works reliably
- [ ] New features delivered incrementally

---

*This file is the single source of truth for project management. Update it whenever project state changes.*
