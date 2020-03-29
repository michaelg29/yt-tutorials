import "package:flutter/material.dart";
import "style/style.dart" as styles;

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: "YouTube tutorial",
      home: HomePage(),
      debugShowCheckedModeBanner: false,
      theme: styles.main,
    );
  }
}

class HomePage extends StatelessWidget {
  void _print() {
    print("Pressed");
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("YouTube Flutter"),
      ),
      body: Center(
        child: Column(
          children: <Widget>[
            Text("Hello, Flutter!"),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        child: Icon(Icons.print),
        onPressed: _print,
      ),
    );
  }
}
