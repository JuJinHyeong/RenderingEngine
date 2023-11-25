"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const express = require("express");
const index_1 = require("./routes/index");
const gpt_1 = require("./routes/gpt");
const app = express();
app.use(express.json({ limit: "10mb" }));
app.use((req, res, next) => { console.log(req.method, req.url); next(); });
app.use('/', index_1.default);
app.use('/gpt', gpt_1.default);
app.use('/health', (req, res) => res.send("still alive..."));
app.use((req, res, next) => {
    const err = new Error('Not Found');
    err['status'] = 404;
    next(err);
});
// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
    app.use((err, req, res, next) => {
        res.status(err['status'] || 500);
    });
}
// production error handler
// no stacktraces leaked to user
app.use((err, req, res, next) => {
    console.log("error!!");
    res.status(err.status || 500);
});
app.set('port', process.env.PORT || 3010);
const server = app.listen(app.get('port'), function () {
    console.log("Server is listening on", app.get('port'), "port");
});
//# sourceMappingURL=app.js.map