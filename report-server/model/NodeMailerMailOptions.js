function NodeMailerMailOptions(from = null, to = null, subject = null, text = null, html = null, cc = null, bcc = null, attachments = null) {
	this.from = from;
	this.to = to;
	this.subject = subject;
	this.text = text;
	this.html = html;
	this.cc = cc;
	this.bcc = bcc;
	this.attachments = attachments;
}
NodeMailerMailOptions.prototype.map = function (object) {
	if (!object) return;
	object.from !== undefined ? this.from = object.from : void (0);
	object.to !== undefined ? this.to = object.to : void (0);
	object.subject !== undefined ? this.subject = object.subject : void (0);
	object.text !== undefined ? this.text = object.text : void (0);
	object.html !== undefined ? this.html = object.html : void (0);
	object.cc !== undefined ? this.cc = object.cc : void (0);
	object.bcc !== undefined ? this.bcc = object.bcc : void (0);
	object.attachments !== undefined ? this.attachments = object.attachments : void (0);
}
NodeMailerMailOptions.prototype.get = function (options = { mandatory: false, optional: false, present: false, compact: false }) {
	let o = {
		from: this.from,
		to: this.to,
		subject: this.subject,
		_text: this.text,
		_html: this.html,
		_cc: this.cc,
		_bcc: this.bcc,
		_attachments: this.attachments
	};
	if (options.mandatory && !options.optional) o = Object.fromEntries(Object.entries(o).filter(_ => !_[0].startsWith('_')));
	if (options.optional && !options.mandatory) o = Object.fromEntries(Object.entries(o).filter(_ => _[0].startsWith('_')));
	if (options.present) o = Object.fromEntries(Object.entries(o).filter(_ => _[1] !== null));
	if (options.compact) o = Object.fromEntries(Object.entries(o).filter(_ => !!_[1]));
	return Object.fromEntries(Object.entries(o).map(_ => { _[0] = _[0].replace('_', ''); return _; }));
}
NodeMailerMailOptions.prototype.getFrom = function () { return this.from }
NodeMailerMailOptions.prototype.getTo = function () { return this.to }
NodeMailerMailOptions.prototype.getSubject = function () { return this.subject }
NodeMailerMailOptions.prototype.getText = function () { return this.text }
NodeMailerMailOptions.prototype.getHtml = function () { return this.html }
NodeMailerMailOptions.prototype.getCc = function () { return this.cc }
NodeMailerMailOptions.prototype.getBcc = function () { return this.bcc }
NodeMailerMailOptions.prototype.getAttachments = function () { return this.attachments }
NodeMailerMailOptions.prototype.setFrom = function (from) { from !== undefined ? this.from = from : void (0) }
NodeMailerMailOptions.prototype.setTo = function (to) { to !== undefined ? this.to = to : void (0) }
NodeMailerMailOptions.prototype.setSubject = function (subject) { subject !== undefined ? this.subject = subject : void (0) }
NodeMailerMailOptions.prototype.setText = function (text) { text !== undefined ? this.text = text : void (0) }
NodeMailerMailOptions.prototype.setHtml = function (html) { html !== undefined ? this.html = html : void (0) }
NodeMailerMailOptions.prototype.setCc = function (cc) { cc !== undefined ? this.cc = cc : void (0) }
NodeMailerMailOptions.prototype.setBcc = function (bcc) { bcc !== undefined ? this.bcc = bcc : void (0) }
NodeMailerMailOptions.prototype.setAttachments = function (attachments) { attachments !== undefined ? this.attachments = attachments : void (0) }
NodeMailerMailOptions.prototype.isEmpty = function () { return !Object.values(this).filter(_ => _ !== null).length }
NodeMailerMailOptions.prototype.isFull = function () { return !Object.values(this).includes(null) }
NodeMailerMailOptions.prototype.isValid = function (options = { mandatory: false, optional: false }) {
	const o = {
		from: this.from,
		to: this.to,
		subject: this.subject,
		_text: this.text,
		_html: this.html,
		_cc: this.cc,
		_bcc: this.bcc,
		_attachments: this.attachments
	};
	let entries = Object.entries(o);
	if (options.mandatory && !options.optional) entries = entries.filter(_ => !_[0].startsWith('_'));
	if (options.optional && !options.mandatory) entries = entries.filter(_ => _[0].startsWith('_'));
	return !Object.values(Object.fromEntries(entries)).includes(null);
}
NodeMailerMailOptions.prototype.listMissingFields = function (options = { mandatory: false, optional: false }) {
	const o = {
		from: this.from,
		to: this.to,
		subject: this.subject,
		_text: this.text,
		_html: this.html,
		_cc: this.cc,
		_bcc: this.bcc,
		_attachments: this.attachments
	};
	let entries = Object.entries(o);
	if (options.mandatory && !options.optional) entries = entries.filter(_ => !_[0].startsWith('_'));
	if (options.optional && !options.mandatory) entries = entries.filter(_ => _[0].startsWith('_'));
	return entries.filter(_ => _[1] === null).map(_ => _[0].replace('_', ''));
}
module.exports = NodeMailerMailOptions;
