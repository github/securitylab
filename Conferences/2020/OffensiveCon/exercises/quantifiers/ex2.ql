import cpp

class InterestingAssignment extends Assignment {
  InterestingAssignment() {
    this.getRValue().getUnderlyingType() != this.getLValue().getUnderlyingType()
  }
}

from InterestingAssignment unused
select unused, unused.getLValue().getUnderlyingType(), unused.getRValue().getUnderlyingType()
