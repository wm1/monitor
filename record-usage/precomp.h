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

const int CHECK_INTERNVAL_IN_SECONDS = 10;
const int FLUSH_INTERVAL_IN_MINUTES  = 15;
