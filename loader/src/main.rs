#[cfg(windows)]
fn main() -> Result<(), String> {
    use std::env;
    use std::ffi::CString;
    use std::mem::{size_of, zeroed};
    use std::path::PathBuf;
    use std::ptr::null_mut;
    use winapi::shared::minwindef::FALSE;
    use winapi::um::handleapi::CloseHandle;
    use winapi::um::libloaderapi::{GetModuleHandleA, GetProcAddress};
    use winapi::um::memoryapi::{VirtualAllocEx, WriteProcessMemory};
    use winapi::um::processthreadsapi::{
        CreateProcessA, CreateRemoteThread, ResumeThread, PROCESS_INFORMATION, STARTUPINFOA,
    };
    use winapi::um::synchapi::WaitForSingleObject;
    use winapi::um::winbase::{CREATE_SUSPENDED, INFINITE};
    use winapi::um::winnt::{MEM_COMMIT, MEM_RESERVE, PAGE_READWRITE};

    let mut args = env::args().skip(1);
    let target_exe = args.next().ok_or_else(|| {
        String::from("usage: loader.exe <path-to-target-exe> [path-to-mouseplay.dll]")
    })?;

    let dll_path = if let Some(path) = args.next() {
        PathBuf::from(path)
    } else {
        let mut default_path = env::current_exe().map_err(|e| e.to_string())?;
        default_path.pop();
        default_path.push("mouseplay.dll");
        default_path
    };
    let dll_path = dll_path
        .canonicalize()
        .map_err(|e| format!("unable to resolve dll path: {}", e))?;

    let target_exe_c = CString::new(target_exe).map_err(|_| "invalid target exe path")?;
    let dll_path_c = CString::new(
        dll_path
            .to_str()
            .ok_or_else(|| String::from("dll path is not valid UTF-8"))?,
    )
    .map_err(|_| String::from("invalid dll path"))?;

    let mut startup_info: STARTUPINFOA = unsafe { zeroed() };
    startup_info.cb = size_of::<STARTUPINFOA>() as u32;
    let mut process_info: PROCESS_INFORMATION = unsafe { zeroed() };

    let create_ok = unsafe {
        CreateProcessA(
            target_exe_c.as_ptr(),
            null_mut(),
            null_mut(),
            null_mut(),
            FALSE,
            CREATE_SUSPENDED,
            null_mut(),
            null_mut(),
            &mut startup_info as _,
            &mut process_info as _,
        )
    };
    if create_ok == 0 {
        return Err(String::from("CreateProcessA failed"));
    }

    let remote_buf = unsafe {
        VirtualAllocEx(
            process_info.hProcess,
            null_mut(),
            dll_path_c.as_bytes_with_nul().len(),
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE,
        )
    };
    if remote_buf.is_null() {
        unsafe {
            CloseHandle(process_info.hThread);
            CloseHandle(process_info.hProcess);
        }
        return Err(String::from("VirtualAllocEx failed"));
    }

    let mut written = 0usize;
    let write_ok = unsafe {
        WriteProcessMemory(
            process_info.hProcess,
            remote_buf,
            dll_path_c.as_ptr() as _,
            dll_path_c.as_bytes_with_nul().len(),
            &mut written as _,
        )
    };
    if write_ok == 0 || written != dll_path_c.as_bytes_with_nul().len() {
        unsafe {
            CloseHandle(process_info.hThread);
            CloseHandle(process_info.hProcess);
        }
        return Err(String::from("WriteProcessMemory failed"));
    }

    let kernel32 = CString::new("kernel32.dll").map_err(|_| "unable to build module name")?;
    let load_library = CString::new("LoadLibraryA").map_err(|_| "unable to build proc name")?;
    let kernel32_handle = unsafe { GetModuleHandleA(kernel32.as_ptr()) };
    if kernel32_handle.is_null() {
        unsafe {
            CloseHandle(process_info.hThread);
            CloseHandle(process_info.hProcess);
        }
        return Err(String::from("GetModuleHandleA(kernel32.dll) failed"));
    }
    let load_library_addr = unsafe { GetProcAddress(kernel32_handle, load_library.as_ptr()) };
    if load_library_addr.is_null() {
        unsafe {
            CloseHandle(process_info.hThread);
            CloseHandle(process_info.hProcess);
        }
        return Err(String::from("GetProcAddress(LoadLibraryA) failed"));
    }

    let remote_thread = unsafe {
        CreateRemoteThread(
            process_info.hProcess,
            null_mut(),
            0,
            Some(std::mem::transmute(load_library_addr)),
            remote_buf,
            0,
            null_mut(),
        )
    };
    if remote_thread.is_null() {
        unsafe {
            CloseHandle(process_info.hThread);
            CloseHandle(process_info.hProcess);
        }
        return Err(String::from("CreateRemoteThread failed"));
    }

    unsafe {
        WaitForSingleObject(remote_thread, INFINITE);
        CloseHandle(remote_thread);
        ResumeThread(process_info.hThread);
        CloseHandle(process_info.hThread);
        CloseHandle(process_info.hProcess);
    }

    println!(
        "injected {} into {}",
        dll_path.display(),
        target_exe_c.to_string_lossy()
    );

    Ok(())
}

#[cfg(not(windows))]
fn main() {
    eprintln!("loader is Windows-only");
}
