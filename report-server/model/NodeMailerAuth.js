function NodeMailerAuth(user = null, pass = null) {
	this.user = user;
	this.pass = pass;
}
NodeMailerAuth.prototype.map = function (object) {
	if (!object) return;
	object.user !== undefined ? this.user = object.user : void (0);
	object.pass !== undefined ? this.pass = object.pass : void (0);
}
NodeMailerAuth.prototype.get = function (options = { mandatory: false, optional: false, present: false, compact: false }) {
	let o = {
		user: this.user,
		pass: this.pass
	};
	if (options.mandatory && !options.optional) o = Object.fromEntries(Object.entries(o).filter(_ => !_[0].startsWith('_')));
	if (options.optional && !options.mandatory) o = Object.fromEntries(Object.entries(o).filter(_ => _[0].startsWith('_')));
	if (options.present) o = Object.fromEntries(Object.entries(o).filter(_ => _[1] !== null));
	if (options.compact) o = Object.fromEntries(Object.entries(o).filter(_ => !!_[1]));
	return Object.fromEntries(Object.entries(o).map(_ => { _[0] = _[0].replace('_', ''); return _; }));
}
NodeMailerAuth.prototype.getUser = function () { return this.user }
NodeMailerAuth.prototype.getPass = function () { return this.pass }
NodeMailerAuth.prototype.setUser = function (user) { user !== undefined ? this.user = user : void (0) }
NodeMailerAuth.prototype.setPass = function (pass) { pass !== undefined ? this.pass = pass : void (0) }
NodeMailerAuth.prototype.isEmpty = function () { return !Object.values(this).filter(_ => _ !== null).length }
NodeMailerAuth.prototype.isFull = function () { return !Object.values(this).includes(null) }
NodeMailerAuth.prototype.isValid = function (options = { mandatory: false, optional: false }) {
	const o = {
		user: this.user,
		pass: this.pass
	};
	let entries = Object.entries(o);
	if (options.mandatory && !options.optional) entries = entries.filter(_ => !_[0].startsWith('_'));
	if (options.optional && !options.mandatory) entries = entries.filter(_ => _[0].startsWith('_'));
	return !Object.values(Object.fromEntries(entries)).includes(null);
}
NodeMailerAuth.prototype.listMissingFields = function (options = { mandatory: false, optional: false }) {
	const o = {
		user: this.user,
		pass: this.pass
	};
	let entries = Object.entries(o);
	if (options.mandatory && !options.optional) entries = entries.filter(_ => !_[0].startsWith('_'));
	if (options.optional && !options.mandatory) entries = entries.filter(_ => _[0].startsWith('_'));
	return entries.filter(_ => _[1] === null).map(_ => _[0].replace('_', ''));
}
module.exports = NodeMailerAuth;
