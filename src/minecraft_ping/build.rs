


use std::{env, path::PathBuf};


fn main() {
    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap() );
    let static_dir = manifest_dir.join("../build/static");
    let shared_dir = manifest_dir.join("../build/shared");



    println!("cargo:rustc-link-search=native={}", static_dir.display());
    println!("cargo:rustc-link-search=native={}", shared_dir.display());


    let static_link = env::var("CARGO_FEATURE_STATIC").is_ok();

    if static_link {
        println!("cargo:rustc-link-lib=static=MinecraftPing");
    }
    else {
        println!("cargo:rustc-link-lib=MinecraftPing");
    }

    println!("cargo:rustc-link-lib=stdc++");

}
