



#[link(name = "MinecraftPing")]

use std::os::raw::*;
use std::ffi::CStr;
use num_derive::FromPrimitive;
use num_traits::FromPrimitive;
use std::fmt;




#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
#[derive(FromPrimitive)]
#[repr(C)]
pub enum c_pingError {
    SOCKET_INITIALIZATION_FAILURE = -11,
    SOCKET_OPEN_FAILURE = -10,
    RECEIVE_FAILURE = -9,
    MALFORMED_VARINT_PACKET = -8,
    INITIALIZATION_FAILURE = -7,
    SEND_FAILURE = -6,
    PING_FAILURE = -5,
    SRV_FAILURE = -4,
    BAD_DOMAIN = -3,
    NO_DOMAIN = -2,
    BAD_RESPONSE = -1,

    CONNECT_FAILURE = 0,
    OK = 1,
    REDIRECTED = 2,
}
/*ping attempt error codes (C layer)*/

#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
#[derive(FromPrimitive)]
#[derive(PartialEq)]
pub enum pingError {
    SOCKET_INITIALIZATION_FAILURE = c_pingError::SOCKET_INITIALIZATION_FAILURE as isize,
    SOCKET_OPEN_FAILURE = c_pingError::SOCKET_OPEN_FAILURE as isize,
    RECEIVE_FAILURE = c_pingError::RECEIVE_FAILURE as isize,
    MALFORMED_VARINT_PACKET = c_pingError::MALFORMED_VARINT_PACKET as isize,
    INITIALIZATION_FAILURE = c_pingError::INITIALIZATION_FAILURE as isize,
    SEND_FAILURE = c_pingError::SEND_FAILURE as isize,
    PING_FAILURE = c_pingError::PING_FAILURE as isize,
    SRV_FAILURE = c_pingError::SRV_FAILURE as isize,
    BAD_DOMAIN = c_pingError::BAD_DOMAIN as isize,
    NO_DOMAIN = c_pingError::NO_DOMAIN as isize,
    BAD_RESPONSE = c_pingError::BAD_RESPONSE as isize,
}
/*ping attempt error codes (Rust layer)*/

#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
#[derive(FromPrimitive)]
#[derive(PartialEq)]
pub enum pingStatus {
    CONNECT_FAILURE = c_pingError::CONNECT_FAILURE as isize,
    OK = c_pingError::OK as isize,
    REDIRECTED = c_pingError::REDIRECTED as isize,
}
/*normal reply values*/



#[repr(C)]
#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
#[derive(PartialEq)]
pub enum DNS_ERROR{NOERROR_STATUS = 0, FORMERR_STATUS = 1, SERVFAIL_STATUS = 2,
                NXDOMAIN_STATUS = 3, NOTIMP_STATUS = 4, REFUSED_STATUS = 5,
                YXDOMAIN_STATUS = 6, XRRSET_STATUS = 7, NOTAUTH_STATUS = 8,
                NOTZONE_STATUS = 9, SEND_REQUEST_FAILURE = 16,
                RECV_REQUEST_FAILURE = 17, WSA_INITIALIZE_FAILURE = 18,
                INVALID_DOMAIN = 19
}
            //SRV DNS server response codes, values 10 thru 15 are reserved


impl fmt::Display for pingStatus {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            pingStatus::CONNECT_FAILURE => write!(f, "Connection refused"),
            pingStatus::OK => write!(f, "Connection established"),
            pingStatus::REDIRECTED => write!(f, "Connection established, redirecting"),
        }
    }
}

impl fmt::Debug for pingStatus {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Display::fmt(self, f)
    }
}



impl fmt::Display for pingError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            pingError::SOCKET_INITIALIZATION_FAILURE => write!(f, "Socket inititialization error"),
            pingError::SOCKET_OPEN_FAILURE => write!(f, "Socket open error"),
            pingError::RECEIVE_FAILURE => write!(f, "Receive error"),
            pingError::MALFORMED_VARINT_PACKET => write!(f, "Malformed varint packet error"),
            pingError::INITIALIZATION_FAILURE => write!(f, "Inititialization error"),
            pingError::SEND_FAILURE => write!(f, "Send error"),
            pingError::PING_FAILURE => write!(f, "Ping error"),
            pingError::SRV_FAILURE => write!(f, "SRV error"),
            pingError::BAD_DOMAIN => write!(f, "Bad domain"),
            pingError::NO_DOMAIN => write!(f, "No domain"),
            pingError::BAD_RESPONSE => write!(f, "Bad response"),
        }
    }

}

impl fmt::Debug for pingError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Display::fmt(self, f)
    }
}



impl fmt::Display for DNS_ERROR {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            DNS_ERROR::NOERROR_STATUS => write!(f, "No error"),
            DNS_ERROR::FORMERR_STATUS => write!(f, "Formatting error"),
            DNS_ERROR::SERVFAIL_STATUS => write!(f, "Server failure"),
            DNS_ERROR::NXDOMAIN_STATUS => write!(f, "Non-existent domain name"),
            DNS_ERROR::NOTIMP_STATUS => write!(f, "Not implemented"),
            DNS_ERROR::REFUSED_STATUS => write!(f, "Refused"),
            DNS_ERROR::YXDOMAIN_STATUS => write!(f, "Name should not exist, but does"),
            DNS_ERROR::XRRSET_STATUS => write!(f, "RRset should not exist, but does"),
            DNS_ERROR::NOTAUTH_STATUS => write!(f, "Server not authorative"),
            DNS_ERROR::NOTZONE_STATUS => write!(f, "Name not in zone"),
            DNS_ERROR::SEND_REQUEST_FAILURE => write!(f, "Failed to send"),
            DNS_ERROR::RECV_REQUEST_FAILURE => write!(f, "Failed to receive"),
            DNS_ERROR::WSA_INITIALIZE_FAILURE => write!(f, "Microsoft WSA failed to init"),
            DNS_ERROR::INVALID_DOMAIN => write!(f, "Invalid domain"),

        }
    }
}

impl fmt::Debug for DNS_ERROR {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Display::fmt(self, f)
    }
}


#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
#[allow(dead_code)]
mod minecraft_ping {

    use std::os::raw::*;
    use super::*;


    pub(super) const DOMAIN_MAX_SIZE: usize = 253;

    #[repr(C)]
    pub(super) struct c_Ping {
        _private: [u8; 0],
    }

    #[repr(C)]
    pub(super) struct c_DNS_Response {
        pub url: [c_char; DOMAIN_MAX_SIZE as usize + 1],
        /*The alias URL of the SRV record. max possible size of a domain
        *url is 253, plus room for terminating null char
        */
        pub dns_error: DNS_ERROR,
        /*DNS error response code*/
        pub port: u16,
        /*redirected port response from the record*/


    }

    extern "C" {

        pub(super) fn newPing() -> *mut c_Ping;

        pub(super) fn createPing(address: *const c_char, p: u16) -> *mut c_Ping;

        pub(super) fn copyPing(obj: *const c_Ping) -> *mut c_Ping;

        pub(super) fn destroyPing(p: *mut c_Ping);

        pub(super) fn ping_connectMC(p: *mut c_Ping ) -> i32;

        pub(super) fn ping_getError(p: *mut c_Ping) -> c_pingError;

        pub(super) fn ping_getResponse(p: *mut c_Ping) -> *mut c_char;

        pub(super) fn ping_getPing(p: *mut c_Ping) -> c_long;

        pub(super) fn ping_SRV_Lookup(domain: *const c_char, dnsr: *mut c_DNS_Response);

        pub(super) fn ping_getDNSerror(p: *mut c_Ping) -> DNS_ERROR;

        pub(super) fn ping_ping_free(p: *mut c_Ping);


    }


}

pub struct Ping{
    ptr: *mut minecraft_ping::c_Ping,
}

pub struct SrvRecord {
    pub url: String,
    pub port: u16,
}


impl Ping {
    pub fn new() -> Self {
        unsafe { Self { ptr: minecraft_ping::newPing() }}
    }

    pub fn create(address: &str, p: u16) -> Result<Self,()> {
        let c_addr = std::ffi::CString::new(address).unwrap();
        let tmp = unsafe { minecraft_ping::createPing(c_addr.as_ptr(), p) };
        if tmp.is_null() {
            Err(())
        }
        else{
            Ok( Self { ptr: tmp })
        }
    }

    pub fn clone(&self) -> Self {
        unsafe { Self { ptr: minecraft_ping::copyPing(self.ptr) }}
    }

    pub fn connect_mc(&self) -> Result<pingStatus, pingError> {

        let ret = unsafe { minecraft_ping::ping_connectMC(self.ptr) };
        if ret as i32 >= 0 {
            Ok(FromPrimitive::from_i32(ret).expect("Not within range of return values"))
        }
        else {
            Err(FromPrimitive::from_i32(ret).expect("Not within range of return values"))
        }
    }

    pub fn get_error(&self) -> Result<pingStatus, pingError> {
        let tmp = unsafe { minecraft_ping::ping_getError(self.ptr) };
        let tmp_i32 = tmp as i32;
        if tmp_i32 > 0 {
            Ok(FromPrimitive::from_i32(tmp_i32).expect("Not within range of return values"))
        }
        else {
            Err(FromPrimitive::from_i32(tmp_i32).expect("Not within range of return values"))
        }
    }

    pub fn get_response(&self) -> String {
        unsafe {
            let raw_str = minecraft_ping::ping_getResponse(self.ptr);
            let c_str = CStr::from_ptr(raw_str);
            c_str.to_string_lossy().into_owned()
        }
    }

    pub fn get_ping(&self) -> i64 {
        unsafe { minecraft_ping::ping_getPing(self.ptr) }
    }

    pub fn get_srv_lookup(domain: &str) -> Result<SrvRecord, DNS_ERROR> {
        let mut c_response = minecraft_ping::c_DNS_Response {
                url: [0 as c_char; minecraft_ping::DOMAIN_MAX_SIZE + 1],
                dns_error: DNS_ERROR::NOERROR_STATUS,
                port: 0,
        };

        let c_domain = std::ffi::CString::new(domain).unwrap();
        unsafe { minecraft_ping::ping_SRV_Lookup(c_domain.as_ptr(), &mut c_response) }

        if c_response.dns_error == DNS_ERROR::NOERROR_STATUS {
            let url = unsafe {
                CStr::from_ptr(c_response.url.as_ptr())
                    .to_string_lossy()
                    .into_owned()
            };
            Ok(SrvRecord {url, port: c_response.port})
        }
        else{
            Err(c_response.dns_error)
        }
    }

    // @todo: remove ping_ping_free(), I dont see the purpose of having it

}

impl Drop for Ping {
    fn drop(&mut self) {
        unsafe {minecraft_ping::destroyPing(self.ptr)}
    }

}




