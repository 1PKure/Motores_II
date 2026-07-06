# Proyecto Side Scroller C++ - Unreal Engine 5

Minijuego side scroller desarrollado en Unreal Engine 5 utilizando C++ como base principal. El proyecto fue realizado para la materia Programación con Motores de Videojuegos II, aplicando conceptos del Gameplay Framework, manejo de UI, enemigos, powerups, flujo completo de partida y estructura de código siguiendo Unreal Coding Standards.

## Descripción

El jugador controla un personaje en un nivel side scroller donde debe avanzar, superar plataformas, utilizar jump pads, evitar peligros, enfrentar enemigos y completar el objetivo del nivel.

El proyecto cuenta con un flujo completo de juego:

- Menú principal.
- Nivel de gameplay.
- HUD funcional.
- Pausa.
- Condiciones de victoria y derrota.
- Pantallas finales con opción de reiniciar o volver al menú.

## Mecánicas principales

- Movimiento del jugador con Enhanced Input.
- Sistema de vida mediante `HealthComponent`.
- HUD con información de vida.
- Enemigos con estados de patrulla, persecución, ataque y muerte.
- Plataformas atravesables configuradas mediante colisiones.
- Jump pads para impulsar al jugador.
- Powerups temporales de movimiento y salto.
- Sistema de pausa.
- Flujo completo de victoria y derrota.
- Base de Gameplay Ability System para habilidad de disparo.

## Gameplay Framework

El proyecto utiliza clases propias de Unreal para separar responsabilidades:

- `AGameplayGameMode`: controla reglas generales de la partida, victoria y derrota.
- `AGamePlayerController`: administra input, HUD, pausa y pantallas finales.
- `APlayerCharacter`: contiene lógica del jugador, vida, powerups y habilidades.
- `AEnemyCharacter`: maneja comportamiento enemigo mediante estados.
- `UHealthComponent`: componente reutilizable para vida, daño y muerte.
- Widgets de UI para HUD, pausa y resultados de partida.

## Enemigos

Los enemigos tienen una máquina de estados simple:

- `Patrol`: patrullan entre puntos definidos en el nivel.
- `Chase`: persiguen al jugador al detectarlo.
- `Attack`: atacan cuando están en rango.
- `Dead`: se desactivan al morir y notifican al GameMode.

La patrulla se configura desde el editor asignando `PatrolPoints` en cada enemigo.

## Controles

| Acción | Input |
|---|---|
| Moverse | A / D |
| Saltar | Space |
| Dash | Shift |
| Disparar | Click izquierdo |
| Pausa | P |

        
## Conceptos aplicados
- Gameplay Framework de Unreal Engine.
- Comunicación entre clases C++.
- Componentes reutilizables.
- Delegates/eventos para muerte y daño.
- Validación de punteros con IsValid.
- Separación de responsabilidades.
- Máquina de estados para enemigos.
- Unreal Coding Standards.
- Configuración de colisiones para plataformas y elementos interactivos.
- Base de GAS para habilidades.
- Estado del proyecto

Autor

Matias Pulido
Tecnicatura Superior en Programación de Videojuegos con Motores
Programación con Motores de Videojuegos II
