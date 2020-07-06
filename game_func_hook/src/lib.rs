extern crate winapi;
extern crate user32;
extern crate kernel32;
extern crate libc;
extern crate detour;

use std::ffi::{OsStr};
use std::iter::once;
use std::os::windows::ffi::OsStrExt;
use kernel32::{GetModuleHandleW};
use winapi::{DWORD, MB_OK, c_void};
use user32::MessageBoxW;
use std::ptr::null_mut;
use detour::static_detour;

fn win32_string(value : &str ) -> Vec<u16> {
    OsStr::new( value ).encode_wide().chain( once( 0 ) ).collect()
}

static_detour! {
    static CwTickFn: unsafe extern "fastcall" fn(DWORD, DWORD, f32) -> c_void;
}

enum GameOffset {
    ClientWorldTick = 0xCAE0,
}

extern "fastcall" fn hooked_cw_tick(p_this: DWORD, _edx: DWORD, float32: f32) -> c_void {
    unsafe {
        MessageBoxW(null_mut(), win32_string("Hello from hooked func").as_ptr(), win32_string("Greetings").as_ptr(), MB_OK);

        return CwTickFn.call(p_this, _edx, float32);
    }
}

type CwTickFnType = unsafe extern "fastcall" fn(DWORD, DWORD, f32) -> c_void;

// entry point
#[no_mangle]
#[allow(non_snake_case, unused_variables)]
pub extern "system" fn DllMain(
    dll_module: winapi::HINSTANCE,
    call_reason: winapi::DWORD,
    reserved: winapi::LPVOID)
    -> winapi::BOOL
{
    const DLL_PROCESS_ATTACH: winapi::DWORD = 1;
    const DLL_PROCESS_DETACH: winapi::DWORD = 0;

    match call_reason {
        DLL_PROCESS_ATTACH => hook_cwtick(),
        DLL_PROCESS_DETACH => (),
        _ => ()
    }

    return winapi::TRUE;
}

fn hook_cwtick() {
    unsafe {
        let module_name = "GameLogic.dll";

        let func_loc: CwTickFnType = hook_game_func(module_name); // "ClientWorld::Tick(float)"

        let hook_closure = | pthis: DWORD, _edx: DWORD, float32: f32 | -> c_void { hooked_cw_tick(pthis, _edx, float32) };

        CwTickFn.initialize(func_loc, hook_closure).unwrap().enable();
    }
}

unsafe fn hook_game_func(module_name: &str) -> CwTickFnType {
    let module_handle = GetModuleHandleW(win32_string(module_name).as_ptr()) as DWORD;

    let func_addr = module_handle + GameOffset::ClientWorldTick as u32;

    let func_address: CwTickFnType = std::mem::transmute::<DWORD, CwTickFnType>(func_addr);

    func_address
}