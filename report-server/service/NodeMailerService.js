const nodemailer = require('nodemailer');
const { NodeMailerAuth, NodeMailerMailOptions, NodeMailerTransportOptions } = require('../model');

function sendEmail(transportOptions = {}, mailOptions = {}, test = false) {
    /** LESS SECURE APPROACH */
    /** Enable less secure apps */
    // https://www.google.com/settings/security/lesssecureapps
    /** Disable captcha temporarily */
    // https://accounts.google.com/b/0/displayunlockcaptcha
    /** SECURE APPROACH */
    /** Enable "2-Step Verification" in "Signing in to Google" */
    // https://myaccount.google.com/security
    // Or
    // https://myaccount.google.com/signinoptions/two-step-verification
    /** Add "App Passwords" as "Other" */
    // https://myaccount.google.com/apppasswords
    return new Promise(async (resolve, reject) => {
        try {
            const transportOpt = new NodeMailerTransportOptions();
            transportOpt.map(transportOptions);
            if (!transportOpt.isValid({ mandatory: true })) {
                throw `transportOptions missing ${transportOpt.listMissingFields({ mandatory: true })}`;
            }
            const auth = new NodeMailerAuth();
            auth.map(transportOpt.getAuth());
            if (!auth.isValid({ mandatory: true })) {
                if (test) {
                    const testAccount = await nodemailer.createTestAccount();
                    auth.setUser(testAccount.user);
                    auth.setPass(testAccount.pass);
                }
                throw `transportOptions.auth missing ${auth.listMissingFields({ mandatory: true })}`;
            }
            const mailOpt = new NodeMailerMailOptions();
            mailOpt.map(mailOptions);
            if (!mailOpt.isValid({ mandatory: true })) {
                throw `mailOptions missing ${mailOpt.listMissingFields({ mandatory: true })}`;
            }
            if (!mailOpt.getText() && !mailOpt.getHtml()) {
                throw `mailOptions.text or mailOptions.html is mandatory`;
            }
            const transporter = nodemailer.createTransport(transportOpt.get({ present: true }));
            const info = await transporter.sendMail(mailOpt.get({ present: true }));
            if (test) {
                return resolve(nodemailer.getTestMessageUrl(info));
            }
            return resolve(info);
        } catch (error) {
            return reject(error);
        }
    });
}

module.exports = {
    sendEmail
};