# Python host application setup

To use the Python host application, go to `tools\wireless\wireless_ranging_host`. This folder is referred later in the document as `<app_folder>`.

This application is a Python application which requires several packages and a specific version of Python, namely 3.6. The supported OS is Windows. It has not been tested on other Python-capable OS.

The recommended and documented approach is to use pipenv** to ease installation of mandatory packages.

If you do not want to use pipenv, you can install Python packages one by one. You can find the list of mandatory packages in `<app_folder>\python\Pipfile`.

**Note:** The wireless ranging host application only works with Python 3.6. Any attempt to use another version causes issues.

To use the Python host application using pipenv, follow the steps below:

**Note:** The following installation procedure is provided for Windows. Use Windows command prompt to execute commands below.

1.  Install [Python 3.6 for 64-bit platform](https://www.python.org/downloads/release/python-368/).
2.  Install pipenv package version 2021.5.29. Note that newer version of pipenv might not be compatible with Python 3.6.

    ```
    pip install 'pipenv==2021.5.29'
    ```

    **Note:**

    -   "python" executable must point to Python 3.6 \(use `python -V` to check the version\).
    -   If using `py` command to manage several Python versions installed on your system, replace "python" with `py -3.6` everywhere in the sections below.
    -   Use `python -m pipenv` instead of pipenv if you did not choose to update your PATH to pipenv executable.
3.  Generate a virtual environment in `<app_folder>/python` folder using Pipfile located there.

    ```
    cd <app_folder>/python
            pipenv --python 3.6 install (or py -3.6 -m pipenv --python 3.6 install)
    ```

    Expected output \(for windows\):

    ```
    Creating a virtualenv for this project…
    Pipfile: <app_folder>\python\Pipfile
    Using <yourPythonPath>/Python36/python.exe (3.6.6) to create virtualenv…
    [=   ] Creating virtual environment...
    [...]
    ```

    Be sure that in the output above, `python.exe` is version 3.6.


This command may take several minutes \(~10 mins\). If pipenv fails \(timeout\) to update packages, adding `--skip-lock` option might work around the problem.

**Parent topic:**[Wireless ranging demo application setup](../topics/wireless_ranging_demo_application_setup.md)

