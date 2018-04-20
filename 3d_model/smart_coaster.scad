$fn=90;
coaster_diameter = 95.8;
coaster_around_width = 1;
lego_hole_diameter = 4.1;
case_around_width = lego_hole_diameter + 2.5 + 3;
case_height = 15;


module bottom() {
  difference() {
    cylinder(d=coaster_diameter, h=6, center=true);
    translate([0, 0, 3]) {
      cylinder(d=coaster_diameter - coaster_around_width, h=6, center=true);
    }
    // Holes for wires
    color("red") translate([(coaster_diameter - coaster_around_width) / 2 - 6, 0, 0]) {
      cube([2, 20, 7], true);
    }
    color("red") translate([(-coaster_diameter - coaster_around_width) / 2 + 6, 0, 0]) {
      cube([2, 20, 7], true);
    }
    // Lego like holes
    for (spin = [40 : 40 : 350])
         rotate(spin) 
         translate([(coaster_diameter - coaster_around_width) / 2 - 2.5,0,-3])
         cylinder(d=lego_hole_diameter,h=5,center=true);
  }
}

module middle() {
 
    // plate
    translate([0, 0, 3]) {
      cylinder(d=coaster_diameter - coaster_around_width - 1, h=1, center=true);
    }
 
}

module case() {

    difference() {
      cylinder(d=coaster_diameter, h=case_height, center=true);
      translate([0, 0, 3])
      cylinder(d=coaster_diameter - case_around_width, h=case_height +1, center=true);

      // Space for usb port
      #rotate(90) translate([-coaster_diameter / 2, -15, -case_height / 2 + 6.7]) {
        cube([20.5, 10.5, 7], center=true);
      }      
    }
    // Lego like cylinders
    for (spin = [40 : 40 : 350])
         rotate(spin) 
         translate([(coaster_diameter - coaster_around_width) / 2 - 2.5, 0, case_height/2])
         cylinder(d=lego_hole_diameter - 0.25,h=5,center=true);

}


!bottom();
translate([0, 0, -case_height]) {
  case();
}
middle();
