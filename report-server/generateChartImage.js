const ChartJsImage = require('chartjs-to-image');

function generateChartImage(data = [], title = 'Line Chart', filePath = 'chart.png') {
	return new Promise(async (resolve, reject) => {
		try {
			const chart = new ChartJsImage();
			const dataY = data.map((item) => item.y);
			const dataX = data.map((item) => item.x);
			chart.setConfig({
				type: 'line',
				data: {
					// labels: Array.from({ length: data.length }, (_, i) => (i + 1) * 20),
					labels: dataX,
					datasets: [
						{
							data: dataY,
							fill: false,
							borderColor: 'rgb(255, 0, 0)'
						}
					]
				},
				options: {
					legend: {
						display: false
					},
					elements: {
						point: {
							radius: 0
						}
					},
					title: {
						display: true,
						text: title
					},
					scales: {
						yAxes: [
							{
								ticks: {
									min: Math.min.apply(this, dataY) - 10,
									max: Math.max.apply(this, dataY) + 10
									// display: false
								}
							}
						],
						xAxes: [
							{
								ticks: {
									autoSkip: true
									// autoSkipPadding: 25,
									// maxRotation: 0
								},
								scaleLabel: {
									display: true,
									labelString: 'Timestamp'
								}
							}
						]
					}
				}
			});
			// chart.toFile(filePath);
			const url = await chart.getShortUrl();
			resolve(url);
		} catch (error) {
			console.error(error);
			reject(error);
		}
	});
}

module.exports = { generateChartImage };
