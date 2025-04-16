# html_generator.py
html_content = """<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Sensor Data</title>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css'>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f4; margin: 0; padding: 20px; }
        .container { max-width: 800px; margin: auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }
        h1 { color: #333; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { border: 1px solid #ddd; padding: 10px; text-align: center; }
        th { background: #007BFF; color: white; }
        tr:nth-child(even) { background: #f9f9f9; }
        .refresh { margin-top: 20px; padding: 10px 20px; background: #007BFF; color: white; border: none; border-radius: 5px; cursor: pointer; }
        .refresh:hover { background: #0056b3; }
    </style>
    <script>
        function refreshData() {
            fetch('/data')
            .then(response => response.json())
            .then(data => {
                let table = document.getElementById('data-table');
                // 清空表格内容，并添加表头
                table.innerHTML = '<tr><th>Water Level</th><th>Moisture</th><th>Light</th><th>Lux</th></tr>';
                data.reverse().forEach(entry => {
                    let row = `<tr>
                        <td>${entry.water_level}</td>
                        <td>${entry.moisture}</td>
                        <td>${entry.light}</td>
                        <td>${entry.lux}</td>
                    </tr>`;
                    table.innerHTML += row;
                });
            })
            .catch(error => console.error('Error fetching data:', error));
        }
        // 每3秒刷新一次数据
        setInterval(refreshData, 3000);
    </script>
</head>
<body