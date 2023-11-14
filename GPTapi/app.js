"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const express = require("express");
const index_1 = require("./routes/index");
const gpt_1 = require("./routes/gpt");
const debug = require('debug')('my express app');
const app = express();
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
        res.render('error', {
            message: err.message,
            error: err
        });
    });
}
// production error handler
// no stacktraces leaked to user
app.use((err, req, res, next) => {
    res.status(err.status || 500);
    res.render('error', {
        message: err.message,
        error: {}
    });
});
app.set('port', process.env.PORT || 3010);
const server = app.listen(app.get('port'), function () {
    debug(`Express server listening on port ${server.address().port}`);
});
//# sourceMappingURL=app.js.map