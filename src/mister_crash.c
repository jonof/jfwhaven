// A crash reports itself to the log (the MiSTer writes no core file; the launcher only
// shows the exit code): the signal, the faulting pc/lr/sp and address from the signal
// context (backtrace() stops at the signal frame on ARM) and the raw backtrace, to
// symbolize with addr2line -e <unstripped build> -f -C -i (non-PIE builds: the
// addresses are the binary's). Installed by a constructor: no call needed. For the
// jfbuild ports (Witchaven, Witchaven II, TekWar, Corridor 8, Skilander), 2026-11-19.
#include <signal.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>
#include <execinfo.h>

static void mister_crash_hex(char *p, unsigned long v)
{
    static const char hex[] = "0123456789abcdef";
    int i;
    for (i = 7; i >= 0; i--) { p[i] = hex[v & 15]; v >>= 4; }
}

static void mister_crash_handler(int sig, siginfo_t *info, void *ctx)
{
    const ucontext_t *uc = (const ucontext_t *)ctx;
    char line[] = "mister crash: SIG???? pc 0x........ lr 0x........ sp 0x........ addr 0x........\n";
    const char *name = sig == SIGSEGV ? "SEGV" : sig == SIGFPE ? "FPE " : sig == SIGBUS ? "BUS " : sig == SIGILL ? "ILL " : sig == SIGABRT ? "ABRT" : "??  ";
    void *frames[32];
    int n, i;
    memcpy(line + 17, name, 4);
#if defined(__arm__)
    mister_crash_hex(line + 27, uc->uc_mcontext.arm_pc);
    mister_crash_hex(line + 41, uc->uc_mcontext.arm_lr);
    mister_crash_hex(line + 55, uc->uc_mcontext.arm_sp);
#else
    (void)uc;
#endif
    mister_crash_hex(line + 71, (unsigned long)info->si_addr);
    if (write(2, line, sizeof(line) - 1) < 0) { }
    n = backtrace(frames, 32);
    for (i = 0; i < n; i++) {
        char f[] = "  frame 0x........\n";
        mister_crash_hex(f + 10, (unsigned long)frames[i]);
        if (write(2, f, sizeof(f) - 1) < 0) { }
    }
    signal(sig, SIG_DFL);
    raise(sig);
}

__attribute__((constructor)) static void mister_crash_install(void)
{
    struct sigaction sa;
    int sigs[] = { SIGSEGV, SIGBUS, SIGILL, SIGFPE, SIGABRT }, i;
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = mister_crash_handler;
    sa.sa_flags = SA_SIGINFO;
    for (i = 0; i < 5; i++) sigaction(sigs[i], &sa, NULL);
}
