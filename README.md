# KeyLogger

## Overview

KeyLogger is a Windows application designed to run as a separate process and record keystrokes made by the user. The application is hidden and does not display any visible windows. Its functionality is based on the use of hooks (interceptors) to intercept keyboard events and monitor window changes.

## Architecture

The entry point for the Win32 application is the `wWinMain` function, where the window class is created, the window is created, and the message processing loop is located.

The architecture of the application includes the following main components:

1. **Main Window:**
   - The main window of the application, which is not displayed on the screen and does not process user messages.
   - A separate thread is created here to send the recorded file to the server.

2. **Interceptors:**
   - **Key Press Interceptor:** Implements the interception of key presses and registers information about them.
   - **Window Change Interceptor:** Monitors changes in the active window and, if the Task Manager is detected, calls the DLL injector program module.

3. **DLL Injector:**
   - A module responsible for injecting a DLL into the Task Manager.
   - If the Task Manager is detected, the DLL injector is called and injects the DLL into the Task Manager process.
   - The injected DLL places a stub on calls to the `NtsystemInformatioQuery` function, which prevents the application from being displayed in the Task Manager and hides it from observation.

4. **DLL Creation Module:**
   - A module responsible for creating the DLL and its functionality.
   - Includes obtaining the function address and intercepting calls to prevent the application from being displayed in the Task Manager.

5. **Server:**
   - The server part that receives the file with the recorded keystrokes and creates copies of them.
   - Ensures the storage and analysis of the received data.

## Multithreading

The development of the KeyLogger uses multithreading, where the main thread is responsible for the operation of the main window, and a separate thread is responsible for sending the recorded file to the server.

## How It Works

1. **Entry Point:**
   - The `wWinMain` function initializes the application, creates the window class, and sets up the message processing loop.

2. **Hidden Window:**
   - The main window is created but remains hidden to avoid detection.

3. **Hooks:**
   - Key press interceptor captures keystrokes.
   - Window change interceptor monitors active window changes and detects the Task Manager.

4. **DLL Injection:**
   - If the Task Manager is detected, the DLL injector injects a DLL into the Task Manager process to hide the KeyLogger from the Task Manager.

5. **Data Transmission:**
   - The recorded keystrokes are sent to the server for storage and analysis.

## Components

- **Main Window:** Hidden window that does not process user messages.
- **Key Press Interceptor:** Captures and records keystrokes.
- **Window Change Interceptor:** Monitors active window changes.
- **DLL Injector:** Injects DLL into the Task Manager to hide the application.
- **DLL Creation Module:** Creates the DLL and its functionality.
- **Server:** Receives and stores the recorded keystrokes.

## Usage

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/IntensioT/KeyLogger.git
   ```

2. **Build the Project:**
   - Open the project in your preferred IDE (e.g., Visual Studio).
   - Build the solution to generate the executable.

3. **Run the Application:**
   - Execute the generated executable.
   - The application will run in the background, recording keystrokes and sending the data to the server.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request if you have any suggestions or improvements.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For any questions or support, please contact the project maintainer:
- Email: [intensioT@gmail.com](mailto:intensioT@gmail.com)
- GitHub: [IntensioT](https://github.com/IntensioT)

---

Stop using malicious software!
