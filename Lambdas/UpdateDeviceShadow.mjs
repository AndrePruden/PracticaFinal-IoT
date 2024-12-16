import pkg from '@aws-sdk/client-iot-data-plane';
import { DynamoDBClient } from '@aws-sdk/client-dynamodb';
import { DynamoDBDocumentClient, ScanCommand } from '@aws-sdk/lib-dynamodb';

const { IoTDataPlaneClient, UpdateThingShadowCommand, GetThingShadowCommand } = pkg;

const client = new IoTDataPlaneClient({ region: 'us-east-1' });
const dynamoDBClient = new DynamoDBClient({ region: 'us-east-1' });
const docClient = DynamoDBDocumentClient.from(dynamoDBClient);

// Configuración de la dirección del endpoint IoT
const iotEndpoint = 'a5ji8fpy1x6e7-ats.iot.us-east-1.amazonaws.com';

async function getThingNameFromDynamoDB() {
    const params = {
        TableName: 'MyThingData',
        Limit: 1
    };

    try {
        const command = new ScanCommand(params);
        const response = await docClient.send(command);
        
        if (response.Items && response.Items.length > 0) {
            return response.Items[0].thing_name;
        }
        throw new Error('No thing name found in DynamoDB');
    } catch (error) {
        console.error('Error retrieving thing name:', error);
        throw error;
    }
}

// Función para obtener el estado actual del Device Shadow
async function getThingShadowState(thingName) {
    const getParams = {
        thingName: thingName,
    };

    try {
        const command = new GetThingShadowCommand(getParams);
        const response = await client.send(command);

        if (response.payload) {
            const payload = JSON.parse(new TextDecoder().decode(response.payload));
            return payload.state.reported;
        }

        throw new Error('Failed to retrieve thing shadow');
    } catch (error) {
        console.error('Error retrieving thing shadow state:', error);
        throw error;
    }
}

export const handler = async (event) => {
    try {
        const thingName = await getThingNameFromDynamoDB();
        
        // Obtener el estado actual del Device Shadow
        const shadowState = await getThingShadowState(thingName);
        
        const autenticado = shadowState.Autenticado || 0;
        
        let updateParams;

        if (autenticado === 0) {
            // Comportamiento actual (si Autenticado es 0)
            updateParams = {
                thingName: thingName,
                payload: JSON.stringify({
                    state: {
                        desired: {
                            servoPosition: 90
                        },
                        reported: {
                            LaserCortado: 0
                        }
                    }
                })
            };
        } else {
            // Si Autenticado es 1, solo actualizamos LaserCortado y Autenticado
            updateParams = {
                thingName: thingName,
                payload: JSON.stringify({
                    state: {
                        reported: {
                            LaserCortado: 0,
                            Autenticado: 0
                        }
                    }
                })
            };
        }

        // Crear el comando para actualizar el Shadow
        const command = new UpdateThingShadowCommand(updateParams);

        // Enviar el comando
        await client.send(command);

        return {
            statusCode: 200,
            body: JSON.stringify('Thing shadow updated successfully')
        };
    } catch (error) {
        console.error('Error updating thing shadow:', error);
        return {
            statusCode: 500,
            body: JSON.stringify('Error updating thing shadow')
        };
    }
};
