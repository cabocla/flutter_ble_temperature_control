import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:numberpicker/numberpicker.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) => MaterialApp(
        debugShowCheckedModeBanner: false,
        title: 'IoT Terrarium',
        theme: ThemeData(
          primarySwatch: Colors.blue,
        ),
        home: MyHomePage(title: 'IoT Terrarium'),
      );
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key key, this.title}) : super(key: key);

  final String title;
  final FlutterBlue flutterBlue = FlutterBlue.instance;
  final List<BluetoothDevice> devicesList =
      new List<BluetoothDevice>.empty(growable: true);
  final Map<Guid, List<int>> readValues = new Map<Guid, List<int>>();

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  static const CHARACTERISTIC_UUID = "910e51f5-ab6f-41cf-a946-305feb203ca4";
  final _writeController = TextEditingController();
  BluetoothDevice _connectedDevice;
  List<BluetoothService> _services;
  BluetoothCharacteristic targetCharacteristic;
  bool clickedRead = false;
  List<String> decodedList;
  List<bool> triggerList;
  List<Map<String, dynamic>> temperatureMapList =
      List<Map<String, dynamic>>.empty(growable: true);

  _addDeviceTolist(final BluetoothDevice device) {
    if (!widget.devicesList.contains(device)) {
      setState(() {
        widget.devicesList.add(device);
      });
    }
  }

  @override
  void initState() {
    super.initState();
    for (int i = 0; i < 5; i++) {
      temperatureMapList.add({
        'triggerOn': false,
      });
    }
    decodedList = ['24 0 0 0 0, 30 30 30 30 30 '];
    triggerList = [false, false, false, false, false];
    widget.flutterBlue.connectedDevices
        .asStream()
        .listen((List<BluetoothDevice> devices) {
      for (BluetoothDevice device in devices) {
        _addDeviceTolist(device);
      }
    });
    widget.flutterBlue.scanResults.listen((List<ScanResult> results) {
      for (ScanResult result in results) {
        _addDeviceTolist(result.device);
      }
    });
    widget.flutterBlue.startScan();
  }

  ListView _buildListViewOfDevices() {
    List<Container> containers = new List<Container>.empty(growable: true);
    for (BluetoothDevice device in widget.devicesList) {
      containers.add(
        Container(
          height: 50,
          child: Row(
            children: <Widget>[
              Expanded(
                child: Column(
                  children: <Widget>[
                    Text(device.name == '' ? '(unknown device)' : device.name),
                    Text(device.id.toString()),
                  ],
                ),
              ),
              ElevatedButton(
                child: Text(
                  'Connect',
                ),
                onPressed: () async {
                  widget.flutterBlue.stopScan();
                  try {
                    await device.connect();
                  } catch (e) {
                    if (e.code != 'already_connected') {
                      throw e;
                    }
                  } finally {
                    _services = await device.discoverServices();
                  }
                  setState(() {
                    _connectedDevice = device;
                  });
                },
              ),
            ],
          ),
        ),
      );
    }

    return ListView(
      padding: const EdgeInsets.all(8),
      children: <Widget>[
        ...containers,
      ],
    );
  }

  List<ButtonTheme> _buildReadWriteNotifyButton(
      BluetoothCharacteristic characteristic) {
    List<ButtonTheme> buttons = new List<ButtonTheme>.empty(growable: true);

    if (characteristic.properties.read) {
      buttons.add(
        ButtonTheme(
          minWidth: 10,
          height: 20,
          child: Padding(
            padding: const EdgeInsets.symmetric(horizontal: 4),
            child: ElevatedButton(
              child: Text('READ', style: TextStyle(color: Colors.white)),
              onPressed: () async {
                clickedRead = !clickedRead;
                var sub = characteristic.value.listen((value) {
                  setState(() {
                    widget.readValues[characteristic.uuid] = value;
                  });
                });
                await characteristic.read();
                sub.cancel();
              },
            ),
          ),
        ),
      );
    }
    if (characteristic.properties.write) {
      buttons.add(
        ButtonTheme(
          minWidth: 10,
          height: 20,
          child: Padding(
            padding: const EdgeInsets.symmetric(horizontal: 4),
            child: ElevatedButton(
              child: Text('WRITE', style: TextStyle(color: Colors.white)),
              onPressed: () async {
                await showDialog(
                    context: context,
                    builder: (BuildContext context) {
                      return AlertDialog(
                        title: Text("Write"),
                        content: Column(
                          mainAxisSize: MainAxisSize.min,
                          children: <Widget>[
                            Text('ex: "1, true" or "2, 25.0"'),
                            Expanded(
                              child: TextField(
                                controller: _writeController,
                              ),
                            ),
                          ],
                        ),
                        actions: <Widget>[
                          TextButton(
                            child: Text("Send"),
                            onPressed: () {
                              updateToESP(_writeController.value.text);
                              Navigator.pop(context);
                            },
                          ),
                          TextButton(
                            child: Text("Cancel"),
                            onPressed: () {
                              Navigator.pop(context);
                            },
                          ),
                        ],
                      );
                    });
              },
            ),
          ),
        ),
      );
    }
    if (characteristic.properties.notify) {
      buttons.add(
        ButtonTheme(
          minWidth: 10,
          height: 20,
          child: Padding(
            padding: const EdgeInsets.symmetric(horizontal: 4),
            child: ElevatedButton(
              child: Text('NOTIFY', style: TextStyle(color: Colors.white)),
              onPressed: () async {
                characteristic.value.listen((value) {
                  widget.readValues[characteristic.uuid] = value;
                });
                await characteristic.setNotifyValue(true);
              },
            ),
          ),
        ),
      );
    }

    return buttons;
  }

  ListView _buildConnectDeviceView() {
    List<Container> containers = new List<Container>.empty(growable: true);

    for (BluetoothService service in _services) {
      List<Widget> characteristicsWidget =
          new List<Widget>.empty(growable: true);

      for (BluetoothCharacteristic characteristic in service.characteristics) {
        characteristicsWidget.add(
          Align(
            alignment: Alignment.centerLeft,
            child: Column(
              children: <Widget>[
                Row(
                  children: <Widget>[
                    Text(characteristic.uuid.toString(),
                        style: TextStyle(fontWeight: FontWeight.bold)),
                  ],
                ),
                Row(
                  children: <Widget>[
                    ..._buildReadWriteNotifyButton(characteristic),
                  ],
                ),
                // Row(
                //   children: <Widget>[
                //     Text('Value: ' +
                //         widget.readValues[characteristic.uuid].toString()),
                //   ],
                // ),
                Divider(),
              ],
            ),
          ),
        );

        if (characteristic.uuid.toString() == CHARACTERISTIC_UUID) {
          targetCharacteristic = characteristic;
          setState(() {});
        }
      }
      containers.add(
        Container(
          child: ExpansionTile(
              title: Text(service.uuid.toString()),
              children: characteristicsWidget),
        ),
      );
    }

    return ListView(
      padding: const EdgeInsets.all(8),
      children: <Widget>[
        !clickedRead
            ? Container(
                height: 0,
                width: 0,
              )
            : StreamBuilder(
                stream: targetCharacteristic?.value,
                builder: (context, snapshot) {
                  var decoded = utf8.decode(snapshot.data);
                  decodedList = decoded.split(',');

                  List<double> temperatureData = decodedList[0]
                      .trim()
                      .split(' ')
                      .map((e) => double.parse(e))
                      .toList();
                  List<double> setTemperatureData = decodedList[1]
                      .trim()
                      .split(' ')
                      .map((e) => double.parse(e))
                      .toList();

                  List<double> customTemperature = [
                    30.7,
                    30.4,
                    30.5,
                    29.8,
                    30.3,
                  ];
                  for (int i = 0; i < temperatureData.length; i++) {
                    temperatureMapList[i].addAll({
                      'index': i + 1,
                      'temperature': customTemperature[i],
                      // 'temperature': temperatureData[i],
                      'setTemperature': setTemperatureData[i],
                    });
                  }

                  List<ListTile> temperatureTiles = temperatureMapList
                      .map((e) => ListTile(
                            title: Text('Temperature: ${e['temperature']}'),
                            subtitle:
                                Text('Set Temperature: ${e['setTemperature']}'),
                            trailing: Row(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                Switch(
                                  value: e['triggerOn'],
                                  onChanged: (value) {
                                    e['triggerOn'] = value;
                                    setState(() {});
                                    String text =
                                        '${e['index']}, ${e['triggerOn']}';
                                    print(text);
                                    updateToESP(text);
                                  },
                                ),
                                IconButton(
                                  icon: Icon(Icons.edit),
                                  onPressed: () async {
                                    await showDialog(
                                      context: context,
                                      builder: (context) {
                                        return NumberPickerDialog.decimal(
                                            minValue: 23,
                                            maxValue: 35,
                                            initialDoubleValue:
                                                e['setTemperature'] == 0
                                                    ? 23
                                                    : e['setTemperature']);
                                      },
                                    ).then((value) {
                                      if (value != null) {
                                        e['setTemperature'] = value;
                                      }
                                    });
                                    String text =
                                        '${e['index']}, ${e['setTemperature']}';
                                    print(text);

                                    updateToESP(text);
                                  },
                                ),
                              ],
                            ),
                          ))
                      .toList();

                  return snapshot.hasData
                      ? Column(
                          children: [...temperatureTiles],
                        )
                      : Container(
                          height: 0,
                          width: 0,
                        );
                },
              ),
        ...containers,
      ],
    );
  }

  ListView _buildView() {
    if (_connectedDevice != null) {
      return _buildConnectDeviceView();
    }
    return _buildListViewOfDevices();
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: AppBar(
          title: Text(widget.title),
        ),
        body: _buildView(),
      );

  void updateToESP(String text) {
    // String newSetTemperature = '';
    // temperatureMapList.forEach((element) {
    //   newSetTemperature += element['setTemperature'].toString();
    //   newSetTemperature += ' ';
    // });
    // newSetTemperature += ',';
    // temperatureMapList.forEach((element) {
    //   if (element['triggerOn']) {
    //     newSetTemperature += 'true';
    //   } else {
    //     newSetTemperature += 'false';
    //   }
    //   newSetTemperature += ' ';
    //   print(element['triggerOn']);
    // });
    targetCharacteristic.write(utf8.encode(text));

    setState(() {});
  }
}
