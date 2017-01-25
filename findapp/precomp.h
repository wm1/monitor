#include <Windows.h>
#include <winnt.h>
#include <winternl.h>
#include <stdio.h>
#include <process.h>
#include <WtsApi32.h>

#include <vector>
#include <string>

#include "timer.h"
#include "win32helper.h"
#include "logger.h"
#include "findapp.h"
