bottom_encl();

module bottom_encl(){
    g_w = 125;
    g_l = 95;
    
    translate([-g_w/2, -g_l/2, 0])
    difference(){
        union(){
        cube([g_w, g_l, 10]);


        }
        union(){
        translate([5, 5, 5])
            cube([g_w-10, g_l-10, 60]);
        }
        
        translate([g_w, g_l, -0.1])
            rotate([0,0,180])
            fillet(60,5);
            
            translate([g_w, 0, -0.1])
            rotate([0,0,90])
            fillet(60,5);
            
       translate([0, g_l, -0.1])
            rotate([0,0,270])
            fillet(60,5);
            
            translate([0, 0, -0.1])
            rotate([0,0,0])
            fillet(60,5);
        
        translate([0, g_l, -0.1])
            rotate([0,90,0])
            rotate([0,0,180])
            fillet(g_w,5);
            
        translate([0, 0, -0.1])
            rotate([0,90,0])
            rotate([0,0,90])
            fillet(g_w,5);
            
        translate([0, g_w, -0.1])
            rotate([90,0,0])
            rotate([0,0,0])
            fillet(g_w,5);
            
        translate([g_w, 0, -0.1])
            rotate([90,0,0])
            rotate([0,0,90])
            fillet(g_w,5);
            
        translate([g_w, 0, -0.1])
            rotate([-90,0,0])
            rotate([0,0,180])
            fillet(g_w,5);
            
        translate([0,1,9])
        rotate([60,0,0])
        translate([0,-22,0])
        cube([130,30,3]);
        
        translate([0,89,17])
        rotate([-60,0,0])
        translate([0,5,0])
        cube([130,30,3]);
        
        translate([0,0,7])
        rotate([0,-60,0])
        cube([30,105,3]);
        
        translate([119,0,17])
        rotate([0,60,0])
        cube([30,100,3]);
    }
    
    translate([-17,-20,5])
    union(){
        cube([4,10,10]);
        translate([68+4,0,0])
        cube([4,10,10]);
        translate([30,-24+56,0])
        cube([10,4,10]);
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