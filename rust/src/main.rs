use std::sync::atomic::{AtomicU32, Ordering};
use std::net::UdpSocket;

static NEXT_ID: AtomicU32 = AtomicU32::new(1);
#[doc="Simple Object Struct to Present Capabilities."]
#[derive(Debug)]
pub struct ExpendableObj {
    pub id: u32, // 4
    pub name: String, // 24
    pub value: i32, // 4
    pub active: bool, // 1
}

impl ExpendableObj {
    pub fn new(name: String, value: i32) -> Self {
        Self {
            id: NEXT_ID.fetch_add(1, Ordering::Relaxed),
            name,
            value,
            active: true,
        }
    }

    pub fn to_bytes(&self) -> Vec<u8> {
        let name_bytes = self.name.as_bytes();
        let mut bytes = Vec::with_capacity(4 + 1 + name_bytes.len() + 4 + 1);


        bytes.extend_from_slice(&self.id.to_le_bytes()); // 4 bytes

        bytes.push(name_bytes.len() as u8); // 1 byte
        bytes.extend_from_slice(name_bytes); // N bytes

        bytes.extend_from_slice(&self.value.to_le_bytes()); // 4 bytes
        bytes.push(self.active as u8); // 1 byte
        bytes
    }

    pub fn from_bytes(bytes: &[u8]) -> Self {
        let id = u32::from_le_bytes(bytes[0..4].try_into().unwrap());

        let name_len = bytes[4] as usize;
        let name_start = 5;
        let name_end = name_start + name_len;

        let name = String::from_utf8_lossy(&bytes[name_start..name_end]).into_owned();
        let value_start = name_end;

        let value = i32::from_le_bytes(bytes[value_start..value_start + 4].try_into().unwrap());
        let active = bytes[value_start + 4] == 1;
        Self {id, name, value, active}
    }
}



fn main() {
    let a = ExpendableObj::new(String::from("A"),56);
    let b = ExpendableObj::new(String::from("B"),80);

    /// Socket - Initializes a port, where Rust will be listening.
    let sock = UdpSocket::bind("127.0.0.1:7878").unwrap();
    println!("Listening on {}", sock.local_addr().unwrap());

    /// A Buffer containing 1024 bytes/ 1KB
    let mut buff = [0u8; 1024];
    loop {
        // Recieves a Msg, Writes it into the buffer and returns the Socket's Port/Address and the received bytes' length.
        let (len, src) = sock.recv_from(&mut buff).expect("Rust Should Recieve Something");
        let c = ExpendableObj::from_bytes(&buff[..len]);
        println!("From {}: {:?}", src, c);
    }
}
