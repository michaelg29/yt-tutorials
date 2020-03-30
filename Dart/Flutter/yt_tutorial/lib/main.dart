import "package:flutter/material.dart";
import "style/style.dart" as styles;

import "utils/navigator.dart" as nav;

import "page2.dart" as page2;

void main() => runApp(MyApp());

List<List<dynamic>> routes = [
  [
    "/",
    "Home",
    (context) => HomePage(),
  ],
  [
    "/p2",
    "Page 2",
    (context, args) => page2.Page2(args: args),
  ]
];

class MyApp extends StatelessWidget {
  MyApp() {
    nav.setRoutes(routes);
  }

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
            RaisedButton(
              child: Text("Goto page 2"),
              onPressed: () {
                nav.sendTo(context, "/p2", {"name": "Name2"});
              },
            )
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        child: Icon(Icons.print),
        onPressed: _print,
      ),
      drawer: Drawer(
          child: nav.buildList(context, [
        "/"
      ], {
        "/p2": {
          "name": "Flutter",
        },
      })),
    );
  }
}
