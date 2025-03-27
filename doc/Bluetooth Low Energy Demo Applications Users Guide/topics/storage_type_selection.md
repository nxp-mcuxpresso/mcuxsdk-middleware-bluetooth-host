# Storage type selection

OTAP Clients support both internal and external storage. By default, the applications come with internal storage enabled. To configure external storage, follow the steps bellow:
1. Set the `gUseInternalStorageLink_d` define to `0` in the project settings.
2. In `app_preinclude.h`, set the `gAppOtaExternalStorage_c` define to `1`.
