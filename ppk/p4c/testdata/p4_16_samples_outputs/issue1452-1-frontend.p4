control c() {
    @name("c.x") bit<32> x_0;
    @name("c.b") action b(@name("arg") out bit<32> arg) {
        arg = 32w2;
    }
    apply {
        b(x_0);
    }
}

control proto();
package top(proto p);
top(c()) main;

