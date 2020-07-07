mod game_declars;

extern crate winapi;
extern crate user32;
extern crate kernel32;
extern crate libc;
extern crate detour;

use std::ffi::{OsStr, CString};
use std::iter::once;
use std::os::windows::ffi::OsStrExt;
use kernel32::{GetModuleHandleW};
use winapi::{DWORD, MB_OK, c_void};
use user32::MessageBoxW;
use std::ptr::null_mut;
use detour::static_detour;
use crate::game_declars::GameDeclarations;

fn win32_string(value : &str ) -> Vec<u16> {
    OsStr::new( value ).encode_wide().chain( once( 0 ) ).collect()
}

// since this is thiscall, the easiest way to detour it is to use fastcall and make the second
// parameter a dead param to not touch edx at all (this pointer comes in ecx)
static_detour! {
    static CwTickFn: unsafe extern "fastcall" fn(DWORD, DWORD, f32) -> c_void;
}

enum GameOffset {
    ClientWorldTick = 0xCAE0,
    PlayerTick = 0x50730
}

extern "fastcall" fn hooked_go_tick(p_this: DWORD, _edx: DWORD, float32: f32) -> c_void {
    unsafe {
        let cw_this_str = format!("value of p_this: {:X?}", p_this);

        MessageBoxW(null_mut(), win32_string(&cw_this_str).as_ptr(), win32_string("Real_p_this").as_ptr(), MB_OK);

        return CwTickFn.call(p_this, _edx, float32);
    }
}

type GameObjTick = unsafe extern "fastcall" fn(DWORD, DWORD, f32) -> c_void;

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

        let func_loc: GameObjTick = hook_game_func(module_name); // "ClientWorld::Tick(float)"

        let hook_closure = | pthis: DWORD, _edx: DWORD, float32: f32 | -> c_void { hooked_go_tick(pthis, _edx, float32) };

        CwTickFn.initialize(func_loc, hook_closure).unwrap().enable();
    }
}

unsafe fn hook_game_func(module_name: &str) -> GameObjTick {
    let module_handle = GetModuleHandleW(win32_string(module_name).as_ptr()) as DWORD;

    let func_addr = module_handle + GameOffset::PlayerTick as u32;

    let func_address: GameObjTick = std::mem::transmute::<DWORD, GameObjTick>(func_addr);

    func_address
}