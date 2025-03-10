const fs = require('fs');

// host build
if ( fs.existsSync("./addon.node") )
    module.exports = require("./addon.node")

else
    throw new Error("unsuppored arch")