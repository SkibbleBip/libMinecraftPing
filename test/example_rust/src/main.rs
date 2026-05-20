
use minecraft_ping::Ping;
use minecraft_ping::pingStatus;
use std::env;
use std::process;



fn main() {

    let args: Vec<String> = env::args().collect();

    if args.len() < 3 {
        println!("Usage: {} [IP] [Port]", args[0]);
        process::exit(1);
    }

    let url = &args[1];

    let port: u16 = args[2].parse().map_err(|_| "Invalid port number").expect("No valid port number");


    let ping = Ping::create(url, port).expect("Failed to init");

    match ping.connect_mc() {
        Err(error) => {
                    eprintln!("Connection error: {}", error);
                    process::exit(error as i32);
            },
        Ok(status) => {

            let msg = match status {
                s @(pingStatus::OK | pingStatus::REDIRECTED) => {

                    let redirected = if matches!(s, pingStatus::REDIRECTED){
                        let ret = Ping::get_srv_lookup(url).expect("Failed to fetch redirected domain");
                        format!("\n\tRedirected: {}:{}", ret.url, ret.port)
                    }else{
                        "".to_string()
                    };


                    let response = ping.get_response();
                    let ms = ping.get_ping();
                    let err = ping.get_error();

                    match err{
                        Ok(value) =>{
                            format!("{}\n\n\tPing: {} ms{}\n\tMinecraft Error code: {}\n\n", response, ms, redirected, value)
                        }
                        Err(value_err) => {
                            format!("{}\n\n\tPing: {} ms{}\n\tMinecraft Error code: {}\n\n", response, ms, redirected, value_err)
                        }
                    }

                }

                pingStatus::CONNECT_FAILURE => {
                    "Failed to connect".to_string()
                }

            }; //match status


            println!("{}", msg);



        }, // Ok

    };






}
