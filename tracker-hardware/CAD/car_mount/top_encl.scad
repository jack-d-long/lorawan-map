use <snap_holder.scad>
use <bottom_encl.scad>

if (1){
    translate([0,0,50])
    rotate([180,0,0])
    bottom_encl();
}
top_encl();

module top_encl(){
    translate([0,0,4])
    snap_holder();
    
    translate([0,-40,2])
    difference(){
     cube([21,21,7]);
     translate([3,3,3])
     cube([15,15,5]);
    }
    
    
    translate([-130/2, -100/2, 0])
    difference(){
        union(){
        cube([130, 100, 30]);
            
        
        
           
            
            
            
        }
        
        union(){
        translate([5, 5, 5])
            cube([120, 90, 60]);
            
        translate([130, 100, -0.1])
            rotate([0,0,180])
            fillet(60,5);
            
            translate([130, 0, -0.1])
            rotate([0,0,90])
            fillet(60,5);
            
       translate([0, 100, -0.1])
            rotate([0,0,270])
            fillet(60,5);
            
            translate([0, 0, -0.1])
            rotate([0,0,0])
            fillet(60,5);
        
        translate([0, 100, -0.1])
            rotate([0,90,0])
            rotate([0,0,180])
            fillet(130,5);
            
        translate([0, 0, -0.1])
            rotate([0,90,0])
            rotate([0,0,90])
            fillet(130,5);
            
        translate([0, 130, -0.1])
            rotate([90,0,0])
            rotate([0,0,0])
            fillet(130,5);
            
        translate([130, 0, -0.1])
            rotate([90,0,0])
            rotate([0,0,90])
            fillet(130,5);
            
        translate([130, 0, -0.1])
            rotate([-90,0,0])
            rotate([0,0,180])
            fillet(130,5);
            
        translate([2,30,22])
            rotate([0,90,0])
            union(){
            cylinder(d=10, h=5, $fn=6);
            translate([0,0,-5])
            cylinder(d=7, h=10, $fn=30);
            }
            
        translate([5,1,32])
        rotate([-60,0,0])
        translate([0,-22,0])
        cube([120,30,3]);
        
        translate([5,89,15])
        rotate([60,0,0])
        translate([0,11,0])
        cube([120,30,3]);
        
        translate([1,5,32])
        rotate([0,60,0])
        cube([30,90,3]);
        
        translate([119,5,15])
        rotate([0,-60,0])
        cube([30,90,3]);
            
        translate([5,5,25])
        cylinder(h=7, d1=0.1, d2=8, $fn=20);
        
        translate([125,5,25])
        cylinder(h=7, d1=0.1, d2=8, $fn=20);
        
        translate([5,95,25])
        cylinder(h=7, d1=0.1, d2=8, $fn=20);
        
        translate([125,95,25])
        cylinder(h=7, d1=0.1, d2=8, $fn=20);
            
        
            
        }       
    }
    
}

module fillet(h, d){
    difference(){
        cylinder(h=h, d=d, $fn=30);
        union(){
            translate([d/2, d/2, -0.1])
            cylinder(h=h+0.2, d=d, $fn=30);
        }
    }
}