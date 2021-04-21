# Rarray GDB printer
#
# This file should be sourced in .gdbinit or copied into ~/.gdbinit.
#
# Ramses van Zon

define printr
    if $argc == 1
        set $rank = sizeof($arg0.shape_.extent_)/sizeof($arg0.shape_.extent_[0])
        whatis $arg0.buffer_.data_[0]
        printf "shape = "
        output $arg0.shape_.extent_._M_elems
        printf "\n"
        if $rank == 1
            output $arg0.buffer_.data_[0]@$arg0.shape_.extent_._M_elems[0]
        end
        if $rank == 2
            output $arg0.buffer_.data_[0]@$arg0.shape_.extent_._M_elems[1]@$arg0.shape_.extent_._M_elems[0]
        end
        if $rank == 3
            output $arg0.buffer_.data_[0]@$arg0.shape_.extent_._M_elems[2]@$arg0.shape_.extent_._M_elems[1]@$arg0.shape_.extent_._M_elems[0]
        end
        if $rank == 4
            output $arg0.buffer_.data_[0]@$arg0.shape_.extent_._M_elems[3]@$arg0.shape_.extent_._M_elems[2]@$arg0.shape_.extent_._M_elems[1]@$arg0.shape_.extent_._M_elems[0]
        end
        if $rank == 5
            output $arg0.buffer_.data_[0]@$arg0.shape_.extent_._M_elems[4]@$arg0.shape_.extent_._M_elems[3]@$arg0.shape_.extent_._M_elems[2]@$arg0.shape_.extent_._M_elems[1]@$arg0.shape_.extent_._M_elems[0]
        end
        if $rank == 6
            output $arg0.buffer_.data_[0]@$arg0.shape_.extent_._M_elems[5]@$arg0.shape_.extent_._M_elems[4]@$arg0.shape_.extent_._M_elems[3]@$arg0.shape_.extent_._M_elems[2]@$arg0.shape_.extent_._M_elems[1]@$arg0.shape_.extent_._M_elems[0]
        end
        printf "\n"                   
    end
end
