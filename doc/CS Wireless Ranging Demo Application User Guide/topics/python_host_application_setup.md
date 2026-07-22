# Python host application setup

To use the Python host application, go to `tools\lcl_host`. This folder is referred later in the document as `<app_folder>`.

This application is a Python application which requires several packages and Python 3.12 or newer. The supported OS is Windows. It has not been tested on other Python-capable OS.

The recommended approach is to install the required packages inside a Python virtual environment. This keeps the packages isolated from your system-wide Python installation and avoids conflicts with existing packages.

The list of required packages is provided in `<app_folder>\python\requirements.txt`. The same packages are also listed in `<app_folder>\python\Pipfile`.

To use the Python host application, follow the steps below:

**Note:** The following installation procedure is provided for Windows. Use Windows command prompt to execute commands below.

1.  Install [Python 3.12 or newer](https://www.python.org/downloads/) for 64-bit platform.

    **Note:** The "python" executable must point to Python 3.12 or newer (use `python -V` to check the version).

2.  Go to the `<app_folder>/python` folder and create a virtual environment.

    ```
    cd <app_folder>/python
    python -m venv .venv
    ```

3.  Activate the virtual environment.

    ```
    source .venv\Scripts\activate
    ```

    After activation, the command prompt is prefixed with `(.venv)`.

4.  Install the required packages inside the virtual environment.

    ```
    pip install -r requirements.txt
    ```

    This command may take several minutes to complete.


When you are done, you can leave the virtual environment by running `deactivate`. Remember to activate the virtual environment again \(step 3\) before running the host application in a new terminal session.

**Parent topic:**[Wireless ranging demo application setup](../topics/wireless_ranging_demo_application_setup.md)
