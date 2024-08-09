void init_dynload(void)
{
#if HAVE_SETDLLDIRECTORY && defined(_WIN32)

    SetDllDirectory("");
#endif
}