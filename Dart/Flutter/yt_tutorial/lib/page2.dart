import "package:flutter/material.dart";

class Page2 extends StatelessWidget {
  Map<String, dynamic> args;

  Page2({this.args});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Page 2"),
      ),
      body: Text(args["name"]),
      floatingActionButton: FloatingActionButton(
        child: Icon(Icons.backspace),
        onPressed: () {
          Navigator.pop(context);
        },
      ),
    );
  }
}
