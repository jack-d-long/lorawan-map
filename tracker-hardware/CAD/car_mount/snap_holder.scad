snap_holder();
cube([107, 6, 3], center=true);
translate([-20,0,0])
cube([6, 77, 3], center=true);

module snap_holder(){
    translate([-23,-77/2-3,-1.5])
    snap_arm();
    translate([-17,+77/2+3,-1.5])
    rotate([0,0,180])
    snap_arm();
    
    translate([107/2+3,-3,-1.5])
    rotate([0,0,90])
    snap_arm();
    
    translate([-107/2-3,3,-1.5])
    rotate([0,0,-90])
    snap_arm();
}

module snap_arm(stand_height=10, lip_height=3){
    cube([6,6,stand_height]);
    cube([6,3,stand_height+lip_height+2]);
    translate([0,0,stand_height+lip_height])
    rotate([90,0,90])
    linear_extrude(6)
    polygon([[0,0], [0,10], [4,0]]);
}