mod game_declars;

extern crate winapi;
extern crate user32;
extern crate kernel32;
extern crate libc;
extern crate detour;

use std::ffi::{OsStr, CString, CStr};
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

static mut haveBeenCalled: bool = false;

extern "fastcall" fn hooked_go_tick(p_this: DWORD, _edx: DWORD, float32: f32) -> c_void {
    unsafe {

        if haveBeenCalled == false {

            let p_copy = p_this;

            haveBeenCalled = true;

            let player_name_loc = p_copy + GameDeclarations::PlayerNameInActivePlayer as u32;

            let player_name_as_mut = std::mem::transmute_copy::<DWORD, *mut i8>(&player_name_loc);

            let player_name = CStr::from_ptr(player_name_as_mut);

            let player_mana_loc = std::mem::transmute::<DWORD, *const u32>(p_copy + GameDeclarations::ManaInPlayer as u32);

            let player_mana = std::ptr::read(player_mana_loc);

            let player_walk_speed_location = std::mem::transmute::<DWORD, *mut f32>(p_copy + GameDeclarations::WalkSpeedInPlayer as u32);

            let player_jump_speed_location = std::mem::transmute::<DWORD, *mut f32>(p_copy + GameDeclarations::JumpSpeedInPlayer as u32);

            let desired_player_walk_speed = 5000.0 as f32;

            let desired_jump_speed = 10000.0 as f32;

            std::ptr::write(player_walk_speed_location, desired_player_walk_speed);

            std::ptr::write(player_jump_speed_location, desired_jump_speed);

            let player_walk_speed = std::ptr::read(player_walk_speed_location);

            let cw_this_str = format!("value of p_this: {:X?}, player name is: {:?}, players current mana is: {:?}, player walk speed is: {:?}", p_copy, player_name, player_mana, player_walk_speed);

            MessageBoxW(null_mut(), win32_string(&cw_this_str).as_ptr(), win32_string("p_this").as_ptr(), MB_OK);
        }

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

        let func_loc: GameObjTick = hook_game_func(module_name); // "Player::Tick(float)"

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