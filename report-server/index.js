const mqtt = require('mqtt');
const { NodeMailerService } = require('./service');
const nodeMailer = {
	transportOptions: {
		host: 'smtp.gmail.com',
		port: 465,
		secure: true,
		auth: {
			user: 'guitarzeed.school.manager@gmail.com',
			pass: 'ddaiqediuwskcpte'
		}
	},
	mailOptions: {
		from: {
			name: 'Data Reporter',
			address: 'guitarzeed.school.manager@gmail.com'
		}
	}
};
const cloudamqp = {
	url: 'mqtt://puffin.rmq2.cloudamqp.com',
	options: {
		username: 'gwbvwhzr:gwbvwhzr',
		password: 'BH4UyDm74GHbzdsYJOFtvZL7LTIM_bNB'
	}
};
const topic = 'app/data';
const client = mqtt.connect(cloudamqp.url, cloudamqp.options);
const store = {};
client.on('connect', function () {
	client.subscribe(topic, function (err) {
		if (!err) {
			client.publish(topic, JSON.stringify({ message: `Report server has subscribed the topic ${topic}` }));
		}
	});
});

client.on('message', function (topic, message) {
	try {
		const payload = JSON.parse(message.toString());
		// console.log({ topic, payload });
		const { id, temp, heart_rate, blood_oxygen, email, name, report_length } = payload;
		if (!id) return;
		const timestamp = new Date().toLocaleString('en-GB').split(', ').join(' ');
		const pppData = { temp, heart_rate, blood_oxygen, timestamp };
		const uid = JSON.stringify({ id, email, name }).split('').sort().join('');
		if (!!store[id]) {
			if (!!store[id].uid) {
				store[id] = {
					...store[id],
					report: [...store[id].report, pppData]
				};
				if (store[id].report.length >= Number(report_length)) {
					console.log({ name, report: store[id].report, email });
					NodeMailerService.sendEmail(nodeMailer.transportOptions, {
						...nodeMailer.mailOptions,
						to: email,
						subject: `Report for ${name}`,
						text: JSON.stringify(store[id].report, null, 2)
					});
					delete store[id];
					client.end();
				}
			} else {
				store[id] = {
					uid,
					report: [pppData]
				};
			}
		} else {
			store[id] = {
				uid,
				report: [pppData]
			};
		}
		// console.log({ store });
	} catch (error) {
		console.error(error);
	}
	// client.end();
});
