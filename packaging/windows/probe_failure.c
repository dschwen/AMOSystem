/* Test-fixture DLL only: force failure to verify rollback of a major upgrade.
 * No MSI APIs or file/registry changes; never included in the AMOS installer. */
__declspec(dllexport) unsigned int __stdcall FailProbe(unsigned int handle)
{
    (void)handle;
    return 1603; /* ERROR_INSTALL_FAILURE */
}
