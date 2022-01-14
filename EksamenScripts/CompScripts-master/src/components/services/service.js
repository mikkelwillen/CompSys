function Frame() {
    this.blockNumber = null
    this.tag = null
    this.content = null
    this.lastAccess = null
    this.fifoindex = null
    this.lruindex = null
}

Frame.prototype.isFull = function () {
    if (this.blockNumber === null) {
        return false
    }
    return true
}



export default function (frameNumber) {
    this.capacity = frameNumber;
    this.placedBlock = 0;
    this.lastOperationIndex = -1;
    this.frames = [];
    for (var i = 0; i < frameNumber; i++) {
        this.frames.push(new Frame());
    }
}
