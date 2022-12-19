function NodeMailerTransportOptions(host = null, port = null, secure = null, auth = null) {
	this.host = host;
	this.port = port;
	this.secure = secure;
	this.auth = auth;
}
NodeMailerTransportOptions.prototype.map = function (object) {
	if (!object) return;
	object.host !== undefined ? this.host = object.host : void (0);
	object.port !== undefined ? this.port = object.port : void (0);
	object.secure !== undefined ? this.secure = object.secure : void (0);
	object.auth !== undefined ? this.auth = object.auth : void (0);
}
NodeMailerTransportOptions.prototype.get = function (options = { mandatory: false, optional: false, present: false, compact: false }) {
	let o = {
		host: this.host,
		_port: this.port,
		_secure: this.secure,
		auth: this.auth
	};
	if (options.mandatory && !options.optional) o = Object.fromEntries(Object.entries(o).filter(_ => !_[0].startsWith('_')));
	if (options.optional && !options.mandatory) o = Object.fromEntries(Object.entries(o).filter(_ => _[0].startsWith('_')));
	if (options.present) o = Object.fromEntries(Object.entries(o).filter(_ => _[1] !== null));
	if (options.compact) o = Object.fromEntries(Object.entries(o).filter(_ => !!_[1]));
	return Object.fromEntries(Object.entries(o).map(_ => { _[0] = _[0].replace('_', ''); return _; }));
}
NodeMailerTransportOptions.prototype.getHost = function () { return this.host }
NodeMailerTransportOptions.prototype.getPort = function () { return this.port }
NodeMailerTransportOptions.prototype.getSecure = function () { return this.secure }
NodeMailerTransportOptions.prototype.getAuth = function () { return this.auth }
NodeMailerTransportOptions.prototype.setHost = function (host) { host !== undefined ? this.host = host : void (0) }
NodeMailerTransportOptions.prototype.setPort = function (port) { port !== undefined ? this.port = port : void (0) }
NodeMailerTransportOptions.prototype.setSecure = function (secure) { secure !== undefined ? this.secure = secure : void (0) }
NodeMailerTransportOptions.prototype.setAuth = function (auth) { auth !== undefined ? this.auth = auth : void (0) }
NodeMailerTransportOptions.prototype.isEmpty = function () { return !Object.values(this).filter(_ => _ !== null).length }
NodeMailerTransportOptions.prototype.isFull = function () { return !Object.values(this).includes(null) }
NodeMailerTransportOptions.prototype.isValid = function (options = { mandatory: false, optional: false }) {
	const o = {
		host: this.host,
		_port: this.port,
		_secure: this.secure,
		auth: this.auth
	};
	let entries = Object.entries(o);
	if (options.mandatory && !options.optional) entries = entries.filter(_ => !_[0].startsWith('_'));
	if (options.optional && !options.mandatory) entries = entries.filter(_ => _[0].startsWith('_'));
	return !Object.values(Object.fromEntries(entries)).includes(null);
}
NodeMailerTransportOptions.prototype.listMissingFields = function (options = { mandatory: false, optional: false }) {
	const o = {
		host: this.host,
		_port: this.port,
		_secure: this.secure,
		auth: this.auth
	};
	let entries = Object.entries(o);
	if (options.mandatory && !options.optional) entries = entries.filter(_ => !_[0].startsWith('_'));
	if (options.optional && !options.mandatory) entries = entries.filter(_ => _[0].startsWith('_'));
	return entries.filter(_ => _[1] === null).map(_ => _[0].replace('_', ''));
}
module.exports = NodeMailerTransportOptions;
