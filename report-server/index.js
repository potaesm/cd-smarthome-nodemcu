const mqtt = require('mqtt');
const { NodeMailerService } = require('./service');
const { generateChartImage } = require('./generateChartImage');
const { renderFile } = require('ejs');
const { join, sep } = require('path');

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

client.on('message', async function (topic, message) {
	try {
		const payload = JSON.parse(message.toString());
		console.log({ topic, payload });
		let { id, temp, heart_rate, blood_oxygen, email, p_name, report_length } = payload;
		if (!id || !temp || !heart_rate || !blood_oxygen) return;
		const timestamp = new Date().toLocaleString('en-GB').split(', ');
		const pppData = { temp, heart_rate, blood_oxygen, timestamp: timestamp[1] };
		const uid = JSON.stringify({ id, email, p_name }).split('').sort().join('');
		if (!!store[id]) {
			if (!!store[id].uid) {
				store[id] = {
					...store[id],
					report: [...store[id].report, pppData]
				};
				if (store[id].report.length >= Number(report_length)) {
					console.log({ p_name, report: store[id].report, email });
					const report = store[id].report;
					const temp_data = report.map((data) => ({ y: Number(data.temp), x: data.timestamp }));
					const heart_rate_data = report.map((data) => ({ y: Number(data.heart_rate), x: data.timestamp }));
					const blood_oxygen_data = report.map((data) => ({ y: Number(data.blood_oxygen), x: data.timestamp }));
					const temp_chart_url = await generateChartImage(temp_data, 'Body Temperature (°C)');
					const heart_rate_chart_url = await generateChartImage(heart_rate_data, 'Heart Rate (BPM)');
					const blood_oxygen_chart_url = await generateChartImage(blood_oxygen_data, 'Blood Oxygen Saturation (%)');
					// console.log({ temp_chart_url, heart_rate_chart_url, blood_oxygen_chart_url });
					await sendReport({ p_name, email, temp_chart_url, heart_rate_chart_url, blood_oxygen_chart_url });
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

function sendReport({ p_name, email, temp_chart_url, heart_rate_chart_url, blood_oxygen_chart_url }) {
	return new Promise(async (resolve, reject) => {
		try {
			const templatePath = join('mail-template', 'report.ejs').split(sep).join('/');
			const html = await renderFile(templatePath, {
				p_name,
				temp_chart_url,
				heart_rate_chart_url,
				blood_oxygen_chart_url
			});
			const result = await NodeMailerService.sendEmail(nodeMailer.transportOptions, {
				...nodeMailer.mailOptions,
				to: email,
				subject: `Health report for ${p_name}`,
				html
			});
			return resolve(result);
		} catch (error) {
			return reject(error);
		}
	});
}
