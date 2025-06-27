# Developer Agent Instructions

Working folder in /Users/fefa4ka/Documents/my/pevi_editor

## Common Rules
1. Project knowledge stored in CLAUDE.md
2. Read first CLAUDE.md
3. Update CLAUDE.md in case of new knowledge
4. **Main tools**: GitHub Issues and Pull Requests
5. ***MANDATORY Code flow**: All code to master ONLY via PR
6. **Team**: Multiple developers work simultaneously

## Specification
1. Plain C code
2. CMake for build

## Core Job
- **Pick up GitHub Issues**: Work on assigned issues
- **Create feature branches**: One branch per issue
- **Submit clean PRs**: Ready for review, no WIP
- **Keep it simple**: Write minimal code that works

## Key Responsibilities
1. **Issue Workflow**
   - Comment on issue when starting work
   - Create feature branch from master
   - Work only on assigned issue scope
   - Update issue with progress if blocked

2. **Code Development**
   - Write clean, simple C code
   - Follow existing code style
   - Add only necessary comments
   - Test your changes locally

3. **Pull Request Workflow**
   - One PR per issue
   - Descriptive PR title and description
   - Link PR to issue number
   - Request review when ready (not before)

## Branch Strategy
- `master` - production ready code
- `feature/issue-123-short-description` - feature branches
- No direct commits to master
- Delete feature branch after PR merge

## What NOT to do
- Don't work on unassigned issues
- Don't create multiple PRs for same issue
- Don't submit draft/WIP PRs
- Don't over-engineer solutions
- Don't add unnecessary features outside issue scope
- Don't commit directly to master

## PR Requirements
- Code compiles with CMake
- Solves the issue completely
- No unrelated changes
- Clear commit messages
- Ready for immediate merge if approved

## When Blocked
- Comment on the issue with specific blocker
- Ask PM for clarification or reassignment
- Don't guess requirements
- Don't work on different issue without assignment
