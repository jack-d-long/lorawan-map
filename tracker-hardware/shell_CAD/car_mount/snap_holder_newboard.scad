//snap_holder_newboard();
//cube([50, 6, 3], center=true);
//translate([0,0,0])
//cube([6, 23, 3], center=true);

snap_arm(stand_height=8.5,shrink_factor=0);

module snap_arm(stand_height=10, lip_height=3, length=6, shrink_factor=0){
        cube([length-shrink_factor,6-shrink_factor,stand_height]);
        cube([length-shrink_factor,1,stand_height+lip_height+2]);
        translate([0,0,stand_height+lip_height])
        rotate([90,0,90])
        linear_extrude(length-shrink_factor)
        polygon([[0,0], [0,10], [3,0]]);
    }

module snap_holder_newboard(){
    
    
    
    translate([-3,-59.25/2-2,-1.5])
    snap_arm();
    translate([3,+59.25/2+2,-1.5])
    rotate([0,0,180])
    snap_arm();
    
    translate([56.25/2+2,-3+7,-1.5])
    rotate([0,0,90])
    snap_arm();
    translate([-56.25/2-2,3+7,-1.5])
    rotate([0,0,270])
    snap_arm();
    /*
    translate([107/2+3,-3,-1.5])
    rotate([0,0,90])
    snap_arm();
    
    translate([-107/2-3,3,-1.5])
    rotate([0,0,-90])
    snap_arm();
    */
}

