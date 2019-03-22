var http = require('http');
var fs = require('fs');
var net = require('net');

const SOCKETFILE = '/tmp/unix.sock';

var app = http.createServer(function(request,response){
    var url = request.url;
    //console.log(url);
    if(request.url == '/'){
      url = '/index.html';
    }
    if(request.url == '/favicon.ico'){
      return response.writeHead(404);
    }
    if(request.url.match('/action/alpha')){
        var req = request.url.split('?')[1];
        if(req){
            dic = req.split('=');
            if(dic[0] == 'value'){
                client.write(dic[1] + '\n');
            }
        } 
        //console.log("good" + parts[1]);
        response.writeHead(200);
        response.end();
        return;
    }
    
    response.writeHead(200);
    //console.log(__dirname + url);
    response.end(fs.readFileSync(__dirname + url));
 
});
app.listen(3000);

var client = net.createConnection(SOCKETFILE)
        .on('connect', ()=>{
            console.log("Connected.");
        })
        // Messages are buffers. use toString
        .on('data', function(data) {
            data = data.toString();

            if(data === '__boop'){
                console.info('Server sent boop. Confirming our snoot is booped.');
                client.write('__snootbooped');
                return;
            }
            if(data === '__disconnect'){
                console.log('Server disconnected.')
                return cleanup();
            }

            // Generic message handler
            console.info('Server:', data)
        })
        .on('error', function(data) {
            console.error('Server not active.'); process.exit(1);
        })
        ;

