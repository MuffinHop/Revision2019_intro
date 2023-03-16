﻿
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode, Serializable, CreateAssetMenu(fileName = "MaterialComponent", menuName = "Material Component")]
public class RM_Material : RM_Surface
{
    public float reflectionCoefficient;
    public Color albedo;
    public float smoothness;
    public float reflectindx;
}