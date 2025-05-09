use <snap_holder_feather.scad>
use <snap_holder_newboard.scad>
use <bottom_encl.scad>

if (0){
    translate([0,0,50])
    rotate([180,0,0])
    bottom_encl();
}
top_encl();

/*
New board is 59x56
*/


module top_encl(){
    boxWidth=100;
    
    
    
    //GPS holder
    translate([18,-27,2])
    difference(){
     cube([21,21,7]);
     translate([3,3,3])
     cube([15,15,5]);
    }
    
    
    /*
    translate([15,-40,2])
    difference(){
     cube([41,52,7]);
     translate([3,3,3])
     cube([35,46,5]);
    }
    
    */
    
    
    translate([-130/2, -(boxWidth+20)/2, 0])
    union(){
        translate([30,-8,0])
        rope_eye(height=30);
        
        translate([30,boxWidth+8,0])
        rotate([0,0,180])
        rope_eye(height=30);  
          
        translate([100,-8,0])
        rope_eye(height=30);
        
        translate([100,boxWidth+8,0])
        rotate([0,0,180])
        rope_eye(height=30); 
    difference(){
        union(){
            cube([130, boxWidth, 30]);  
        }
        
        union(){
        //translate([-29,-15,4])
        translate([37,boxWidth/2-10,3])
        rotate([0,0,0])
        snap_holder_newboard();    
            
        translate([5, 5, 5])
            cube([120, boxWidth-10, 60]);
            
        translate([130, boxWidth, -0.1])
            rotate([0,0,180])
            fillet(boxWidth-20,5);
            
            translate([130, 0, -0.1])
            rotate([0,0,90])
            fillet(boxWidth-20,5);
            
       translate([0, boxWidth, -0.1])
            rotate([0,0,270])
            fillet(boxWidth-20,5);
            
            translate([0, 0, -0.1])
            rotate([0,0,0])
            fillet(boxWidth-20,5);
        
        translate([0, boxWidth, -0.1])
            rotate([0,90,0])
            rotate([0,0,180])
            fillet(130,5);
            
        translate([0, 0, -0.1])
            rotate([0,90,0])
            rotate([0,0,90])
            fillet(130,5);
            
        translate([0, 110, -0.1])
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
        
        translate([5,boxWidth-11,15])
        rotate([60,0,0])
        translate([0,11,0])
        cube([120,30,3]);
        
        translate([1,5,32])
        rotate([0,60,0])
        cube([30,boxWidth-10,3]);
        
        translate([119,5,15])
        rotate([0,-60,0])
        cube([30,boxWidth-10,3]);
            
        translate([5,5,25])
        cylinder(h=7, d1=0.1, d2=8, $fn=20);
        
        translate([125,5,25])
        cylinder(h=7, d1=0.1, d2=8, $fn=20);
        
        translate([5,boxWidth-5,25])
        cylinder(h=7, d1=0.1, d2=8, $fn=20);
        
        translate([125,boxWidth-5,25])
        cylinder(h=7, d1=0.1, d2=8, $fn=20);
            
        
            
        }       
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

module rope_eye(height=10) {
    translate([-10,0,0])
    difference(){
        union(){
            translate([10,0,0])
            cylinder(h=height,d=20, $fn=30);
            cube([20,10,height]);
        }
        translate([10,0,-1])
        cylinder(h=height+2, d=12, $fn=30);
    }
}