# xRat System v3 - Cliente

**CRITICAL**: Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.

xRat System v3 - Cliente is a native Windows C++ application with MVC architecture for system monitoring and control. It uses pure Win32 API without external dependencies and includes background window monitoring functionality.

## Working Effectively

### Prerequisites and Setup
- Install MinGW-w64 cross-compiler for Windows targets:
  ```bash
  sudo apt update && sudo apt install -y mingw-w64 mingw-w64-tools
  ```
- Verify installation:
  ```bash
  x86_64-w64-mingw32-g++ --version
  ```

### Build Process
Always create build directories first:
```bash
mkdir -p obj obj/utils obj/forms obj/controllers bin
```

#### Option 1: Using Makefile (Recommended)
```bash
# Cross-compile from Linux (VALIDATED - works perfectly):
make CXX=x86_64-w64-mingw32-g++ clean all
```
- **Build time**: 1-8 seconds. NEVER CANCEL. Set timeout to 30+ seconds minimum.
- **Debug build**: `make CXX=x86_64-w64-mingw32-g++ clean debug` - takes 1-2 seconds
- **Release build**: `make CXX=x86_64-w64-mingw32-g++ clean release` - takes 1-8 seconds
- **Info display**: `make CXX=x86_64-w64-mingw32-g++ info` - shows project details
- **Incremental builds**: ~12ms when no changes detected

#### Option 2: Manual Cross-Compilation (Linux)
```bash
# Compile all modules (takes ~6 seconds total):
x86_64-w64-mingw32-g++ -std=c++11 -Wall -O2 -Iinclude -c src/utils/AppUtils.cpp -o obj/utils/AppUtils.o
x86_64-w64-mingw32-g++ -std=c++11 -Wall -O2 -Iinclude -c src/utils/WindowMonitor.cpp -o obj/utils/WindowMonitor.o
x86_64-w64-mingw32-g++ -std=c++11 -Wall -O2 -Iinclude -c src/controllers/MainController.cpp -o obj/controllers/MainController.o
x86_64-w64-mingw32-g++ -std=c++11 -Wall -O2 -Iinclude -c src/forms/MainForm.cpp -o obj/forms/MainForm.o
x86_64-w64-mingw32-g++ -std=c++11 -Wall -O2 -Iinclude -c src/main.cpp -o obj/main.o

# Link final executable (takes ~32ms):
x86_64-w64-mingw32-g++ obj/main.o obj/utils/AppUtils.o obj/utils/WindowMonitor.o obj/controllers/MainController.o obj/forms/MainForm.o -o bin/main.exe -lgdi32 -luser32 -lkernel32
```

#### Option 3: Windows Native Build (Windows only)
```cmd
# Using the provided batch script:
.\build_mvc.bat
```

**CRITICAL BUILD WARNINGS**:
- **NEVER CANCEL** builds - they complete in under 10 seconds typically
- Set timeouts to minimum 30 seconds for any build command
- Cross-compilation from Linux works perfectly and is faster than Windows native builds
- Expected output: PE32+ executable ~280KB in `bin/main.exe`

### Testing and Validation
Since this is a Windows GUI application that cannot be executed directly on Linux, validation is limited to:

#### Build Validation (Always run these steps):
```bash
# Verify executable was created:
ls -la bin/main.exe
file bin/main.exe  # Should show: "PE32+ executable (console) x86-64, for MS Windows"

# Test that trying to run on Linux fails as expected:
timeout 5 ./bin/main.exe 2>&1 | head -2  # Should show "not found" or syntax error
```

#### Size Validation:
- Release build: ~220KB
- Debug build: ~284KB
- File type: PE32+ executable (console) x86-64, for MS Windows, 19 sections

#### Manual Testing Scenarios (Windows required):
**CRITICAL**: When testing on Windows, you MUST run through these complete scenarios:
1. **Application Startup Test**:
   - Execute `bin/main.exe`
   - Verify main window appears centered with buttons: "Informações", "Sobre", "Configurações", "Sair"
   - Check that `bin/application.log` is created

2. **Interface Interaction Test**:
   - Click each button: Informações, Sobre, Configurações
   - Verify appropriate dialogs/windows appear
   - Check logs are written to `bin/application.log`

3. **Background Monitoring Test**:
   - Let application run for 30+ seconds
   - Open/close other applications (notepad, calculator)
   - Verify `bin/application.log` contains window monitoring entries every 2 seconds
   - Check log entries show window titles and HWND handles

4. **Clean Shutdown Test**:
   - Click "Sair" button
   - Verify application closes gracefully
   - Check final log entries for proper shutdown sequence

### CI/CD Integration
The repository includes validated GitHub Actions workflows:

#### Build Workflow (.github/workflows/build.yml)
- Triggers on push to master/develop and PRs to master
- Tests both MinGW and MSVC builds on Windows runners
- **Build time**: 2-5 minutes. NEVER CANCEL. Set timeout to 10+ minutes.
- Uploads build artifacts with 30-day retention

#### Release Workflow (.github/workflows/release.yml)
- Triggers on git tags (e.g., `git tag v1.0.1 && git push origin v1.0.1`)
- Creates releases with both MinGW and MSVC binaries
- **Release time**: 5-10 minutes. NEVER CANCEL. Set timeout to 15+ minutes.

## Repository Structure

### Source Code Organization
```
src/                          # Source files
├── controllers/              # Business logic
│   └── MainController.cpp    # Main application controller
├── forms/                    # Win32 GUI interface
│   └── MainForm.cpp          # Main window implementation
├── utils/                    # Shared utilities
│   ├── AppUtils.cpp          # Common utilities (logging, UI helpers)
│   └── WindowMonitor.cpp     # Background window monitoring
└── main.cpp                  # Application entry point

include/                      # Header files (mirror structure)
├── controllers/
├── forms/
├── utils/
└── main.h

bin/                          # Generated executables
obj/                          # Compiled object files
.github/                      # GitHub configuration
├── workflows/                # CI/CD pipelines
└── ISSUE_TEMPLATE/           # Issue templates
```

### Key Files
- **Makefile**: Primary build system (supports both native GCC and cross-compilation)
- **Makefile.mvc**: Alternative modular build system
- **build_mvc.bat**: Windows batch script for native builds
- **README.md**: Portuguese documentation for the project
- **.github-copilot-instructions.md**: Legacy instructions (superseded by this file)

## Architecture and Code Patterns

### MVC Architecture
- **Model**: Data structures in `ApplicationData` and utilities
- **View**: Win32 GUI in `src/forms/MainForm.cpp`
- **Controller**: Business logic in `src/controllers/MainController.cpp`

### Coding Conventions
- **Language**: C++11 standard, Win32 API, no external dependencies
- **Logging**: Always use `AppUtils::WriteLog("message", "LEVEL")` for application logs
- **Debug**: Use `AppUtils::DebugPrint("message")` for debug output
- **UI Messages**: All user-facing text in Portuguese (PT-BR)
- **Error Handling**: Use `AppUtils::ShowErrorMessage()` for critical failures
- **Memory Management**: RAII patterns, proper resource cleanup in destructors

### Module Responsibilities
- **AppUtils**: Logging, UI helpers, date/time functions, window management
- **WindowMonitor**: Background thread monitoring active windows every 2 seconds
- **MainController**: Application lifecycle, module coordination, business logic
- **MainForm**: Win32 window creation, event handling, UI layout
- **main.cpp**: Entry point, module initialization/shutdown

## Development Guidelines

### Adding New Features
1. **Always follow MVC separation**:
   - Business logic → Controller
   - UI changes → Form
   - Shared functionality → Utils

2. **Update build files** when adding new source files:
   - Add to `SOURCES` in Makefile
   - Add compilation step in `build_mvc.bat`
   - Include new headers in appropriate directories

3. **Logging requirements**:
   - Log all significant events using `AppUtils::WriteLog`
   - Use levels: "INFO", "WARNING", "ERROR", "DEBUG", "SYSTEM"
   - Include timing for performance-critical operations

### Common Development Tasks

#### Adding a New Utility Function
1. Add declaration to `include/utils/AppUtils.h`
2. Implement in `src/utils/AppUtils.cpp`
3. Use `AppUtils::` namespace prefix

#### Adding a New UI Element
1. Define control ID in `FormConstants`
2. Create control in `MainForm::InitializeComponents`
3. Handle events in `MainForm::WindowProc`
4. Update controller if business logic is needed

#### Adding Background Processing
1. Follow `WindowMonitor` pattern for thread management
2. Always use proper thread synchronization
3. Implement graceful shutdown in thread cleanup

### Troubleshooting

#### Build Issues
- **"g++ not found"**: Install MinGW: `sudo apt install mingw-w64`
- **"invalid-compiler: No such file or directory"**: Specify correct cross-compiler: `make CXX=x86_64-w64-mingw32-g++`
- **"Permission denied"**: Ensure build directories exist: `mkdir -p obj bin`
- **"Undefined reference"**: Check link libraries: `-lgdi32 -luser32 -lkernel32`
- **Windows-specific headers**: Use `#include <windows.h>` for Win32 API
- **Make command not found**: Install build-essential: `sudo apt install build-essential`

#### Runtime Issues (Windows testing)
- **Application won't start**: Check application.log for initialization errors
- **No logs created**: Verify write permissions in bin/ directory
- **Window monitoring not working**: Check WindowMonitor thread status in logs
- **UI not responding**: Ensure long operations run in separate threads

#### Cross-compilation Issues
- **"target not found"**: Ensure MinGW is properly installed and in PATH
- **Missing Windows libraries**: Check that `-lgdi32 -luser32 -lkernel32` are in link command
- **Invalid executable format**: Use `file bin/main.exe` to verify PE32+ format

## Issue Management

### Bug Reports
Use `.github/ISSUE_TEMPLATE/bug_report.yml`:
- Include application.log contents
- Specify Windows version and build type (MinGW/MSVC)
- Provide clear reproduction steps

### Feature Requests  
Use `.github/ISSUE_TEMPLATE/feature_request.yml`:
- Categorize by area: UI, Monitoring, Logs, Performance, etc.
- Specify priority level
- Consider MVC architecture impact

### Pull Request Process
Follow `.github/pull_request_template.md` checklist:
- Test both MinGW and MSVC builds if possible
- Verify Debug and Release configurations
- Ensure MVC separation is maintained
- Update documentation if needed

## Performance Notes
- **Startup time**: ~500ms typical
- **Memory usage**: ~2-4MB runtime
- **Window monitoring**: 2-second intervals (configurable in WindowMonitor)
- **Log file growth**: ~1KB per hour of operation

**CRITICAL TIMEOUT GUIDANCE**:
- Build commands: Minimum 30 seconds timeout
- Test runs: Minimum 60 seconds timeout  
- CI/CD pipelines: Minimum 15 minutes timeout
- **NEVER CANCEL** any operation before timeout expires