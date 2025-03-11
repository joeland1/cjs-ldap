const fs = require('fs');
const path = require("path");
const os = require('os');

const HOST_BUILD_LOC = path.resolve(__dirname, "./addon.node");
const PREBUILT_LOC = path.resolve(__dirname, "./addon.node");

// host build
const thing_to_export = (()=>{

    if ( fs.existsSync(HOST_BUILD_LOC) )
        return require( HOST_BUILD_LOC )

    const files = fs.readdirSync(path.resolve(__dirname, "./prebuilt"));
    
    for(let i=0;i< files.length;i++){
        const fname = path.resolve(__dirname+"/prebuilt", files[i]);
        console.log('testing',fname)
        try {
            const x = require(fname);
            return x;
        }
        catch( e ) { console.log(e) }
    }
    return false
})()


if (!thing_to_export)
    throw new Error("unsuppored arch")

module.exports = thing_to_export;