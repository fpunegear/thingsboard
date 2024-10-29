// Ejemplo de json enviado por suela
// {
//     "id": "ASD123",
//     "side": "right"|"left",
//     "values": [
//         {
//             "weight": 10,
//             "positionX": 1,
//             "positionY": 1
//         },
//         {
//             "weight": 11,
//             "positionX": 1,
//             "positionY": 2
//         },
//         {
//             "weight": 10,
//             "positionX": 1,
//             "positionY": 3
//         },
//         {
//             "weight": 9,
//             "positionX": 2,
//             "positionY": 3
//         },
//         {
//             "weight": 8,
//             "positionX": 4,
//             "positionY": 3
//         },
//         {
//             "weight": 13,
//             "positionX": 2,
//             "positionY": 5
//         }
//     ]
// }

var sumWeight = 0;
var sumPositionX = 0;
var sumPositionY = 0;
var xArray = [];
var yArray = [];
var higherPressureValue = msg.values[0];
var typeOfFootprint = "";

for (var i = 0; i < msg.values.length; i++) {
    var value = msg.values[i];
    sumWeight += value.weight;
    sumPositionX += value.positionX * value.weight;
    sumPositionY += value.positionY * value.weight;

    // populate positions array
    xArray.push(value.positionX);
    yArray.push(value.positionY);

    // find the high pressure
    if (value.weight > higherPressureValue.weight) {
        higherPressureValue = value;
    }
}

// Verificar que sumWeight no sea 0 para evitar división por 0
var centerMassX = sumWeight && sumPositionX / sumWeight;
var centerMassY = sumWeight && sumPositionY / sumWeight;

// Calcular la medida del pie
var minX = Math.min.apply(null, xArray);
var maxX = Math.max.apply(null, xArray);
var minY = Math.min.apply(null, yArray);
var maxY = Math.max.apply(null, yArray);

var footLength = maxX;
var footWidth = maxY;

// Determinar tipo de pisada
var pronationLimit = footLength * 0.3;
var supinationLimit = footLength * 0.7;

if (centerMassX < pronationLimit) {
    typeOfFootprint = "Sobrepronación";
} else if (centerMassX < pronationLimit * 1.1) {
    typeOfFootprint = "Pisada pronadora";
} else if (centerMassX > supinationLimit) {
    typeOfFootprint = "Sobresupinación";
} else if (centerMassX > supinationLimit * 0.9) {
    typeOfFootprint = "Pisada supinadora";
} else {
    typeOfFootprint = "Pisada neutra";
}

return {
    msg: { 
        id: msg.id,
        side: msg.side,
        values: msg.values,
        centerOfMass: [centerMassX, centerMassY],
        footLength: footLength,
        footWidth: footWidth,
        typeOfFootprint: typeOfFootprint,
        higherPressureValue: higherPressureValue
    }, 
    metadata: metadata, 
    msgType: msgType
};