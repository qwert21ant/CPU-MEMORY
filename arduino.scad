size = [100, 30, 30]; //размер корпуса
weight = 2; //толщина корпуса

board = [43.2, 17.8, 4]; //размеры Arduino Nano (~высота до usb разъёма)
usb = [6.8, 3]; //размер usb разъёма

color([50/255, 50/255, 50/255, 1]){
face([size.x, size.y]);
face([size.x, size.z], [0, size.y, 0], [90, 0, 0]);
difference(){
    tringle([0, 0, weight]);
    translate([0, (size.y - weight - board.y) + (board.y - usb.x) / 2, weight + board.z])
        cube([weight, usb.x, usb.y]);
}
tringle([size.x - weight, 0, weight]);
}

//arduino nano
color([255/255, 0/255, 0/255, 1])
    translate([weight, size.y - weight - board.y, weight])
    cube(board);

rotate([45, 0, 0]){
    difference(){
        face([size.x, size.z * sqrt(2)], w=4);
        translate([10, 6.2, weight]) face([size.x * 0.8, size.z * sqrt(2) * 0.707]);
        $fn = 20;
        for(i = [0:7]){
            dCyl([41.5 + i * 6, 25.66, 0]);
        }
        
        for(i = [0:7]){
            dCyl([41.5 + i * 6, 10.66, 0]);
        }
    }
  
    translate([15.375, 24.7, 2]) linear_extrude(2) text("CPU", size=8);
    translate([13.805, 10.16, 2]) linear_extrude(2) text("MEM", size=8);
}

module dCyl(pos = [0, 0, 0]){
    translate(pos) translate([3, 3, 0]){
    cylinder(1, d = 6, center = false);
    translate([0, 0, 1]) cylinder(1, d = 5, center = false);
    }
}

module face(size = [1, 1], pos = [0, 0, 0], rot = [0, 0, 0], w = weight){
    translate(pos) rotate(rot)
        cube([size.x, size.y, w], false);
}

module tringle(pos){
    translate(pos)
    difference(){
        cube([weight, size.y - weight, size.z - weight]);
        rotate([45, 0, 0])
            cube([weight, size.y * 1.5, size.z]);
    }
}