--
-- PostgreSQL database dump
--

-- Dumped from database version 13.2
-- Dumped by pg_dump version 13.2

-- Started on 2021-06-06 18:21:57

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- TOC entry 3 (class 2615 OID 16411)
-- Name: power_grid; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA power_grid;


ALTER SCHEMA power_grid OWNER TO postgres;

--
-- TOC entry 218 (class 1255 OID 32968)
-- Name: insert_into_aclinesegment(uuid, character varying, integer, integer, integer, uuid, uuid); Type: PROCEDURE; Schema: power_grid; Owner: postgres
--

CREATE PROCEDURE power_grid.insert_into_aclinesegment(id uuid, nm character varying, leng integer, secti integer, transmi_pow integer, outp uuid, inp uuid)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF ((SELECT "uuid_TP" FROM power_grid."T_substation" WHERE "uuid_TP" = outp) IS NULL AND (SELECT "uuid_RP" FROM power_grid."D_substation" WHERE "uuid_RP" = outp) IS NULL) THEN
		RAISE EXCEPTION 'Нет такого источника';
	ELSEIF ((SELECT "uuid_TP" FROM power_grid."T_substation" WHERE "uuid_TP" = inp) IS NULL AND (SELECT "uuid_RP" FROM power_grid."D_substation" WHERE "uuid_RP" = inp) IS NULL) THEN
		RAISE EXCEPTION 'Нет такого потреюителя';
	ELSEIF ((SELECT "uuid_TP" FROM power_grid."T_substation" WHERE "uuid_TP" = outp) IS NOT NULL AND (SELECT "uuid_RP" FROM power_grid."D_substation" WHERE "uuid_RP" = inp) IS NOT NULL) THEN
		RAISE EXCEPTION 'ТП не может быть источником для РП';
	ELSEIF (outp = inp) THEN 
		RAISE EXCEPTION 'Кабель не может приходить в тот-же объект';
	ELSE 
		UPDATE "ACLineSegment" SET ("name", "length", section, transmitted_power, output, input) = ("nm", leng, secti, transmi_pow, outp, inp) WHERE "uuid_cable" = id;
		IF NOT FOUND THEN 
			INSERT INTO "ACLineSegment" VALUES (id, "nm", leng, secti, transmi_pow, outp, inp);
		END IF;
	END IF;
END;
$$;


ALTER PROCEDURE power_grid.insert_into_aclinesegment(id uuid, nm character varying, leng integer, secti integer, transmi_pow integer, outp uuid, inp uuid) OWNER TO postgres;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- TOC entry 206 (class 1259 OID 32957)
-- Name: ACLineSegment; Type: TABLE; Schema: power_grid; Owner: postgres
--

CREATE TABLE power_grid."ACLineSegment" (
    uuid_cable uuid DEFAULT gen_random_uuid() NOT NULL,
    name character varying(254) NOT NULL,
    length integer NOT NULL,
    section integer NOT NULL,
    transmitted_power integer NOT NULL,
    output uuid NOT NULL,
    input uuid NOT NULL
);


ALTER TABLE power_grid."ACLineSegment" OWNER TO postgres;

--
-- TOC entry 205 (class 1259 OID 32944)
-- Name: Cim_model_nodes; Type: TABLE; Schema: power_grid; Owner: postgres
--

CREATE TABLE power_grid."Cim_model_nodes" (
    attributes character varying(254) NOT NULL,
    class character varying(254),
    link_source character varying(254),
    name_column character varying(254) NOT NULL,
    resurce_or_value integer,
    const_value character varying(254)
);


ALTER TABLE power_grid."Cim_model_nodes" OWNER TO postgres;

--
-- TOC entry 204 (class 1259 OID 32938)
-- Name: D_substation; Type: TABLE; Schema: power_grid; Owner: postgres
--

CREATE TABLE power_grid."D_substation" (
    "uuid_RP" uuid DEFAULT gen_random_uuid() NOT NULL,
    name character varying(254),
    coordinates point NOT NULL
);


ALTER TABLE power_grid."D_substation" OWNER TO postgres;

--
-- TOC entry 201 (class 1259 OID 24757)
-- Name: Diesel_generator; Type: TABLE; Schema: power_grid; Owner: postgres
--

CREATE TABLE power_grid."Diesel_generator" (
    "uuid_DGU" uuid DEFAULT gen_random_uuid() NOT NULL,
    power integer NOT NULL,
    coordinates point NOT NULL,
    uuid_platform uuid NOT NULL
);


ALTER TABLE power_grid."Diesel_generator" OWNER TO postgres;

--
-- TOC entry 200 (class 1259 OID 24751)
-- Name: Platform_DGU; Type: TABLE; Schema: power_grid; Owner: postgres
--

CREATE TABLE power_grid."Platform_DGU" (
    uuid_platform uuid DEFAULT gen_random_uuid() NOT NULL,
    coordinates point NOT NULL
);


ALTER TABLE power_grid."Platform_DGU" OWNER TO postgres;

--
-- TOC entry 203 (class 1259 OID 32883)
-- Name: T_substation; Type: TABLE; Schema: power_grid; Owner: postgres
--

CREATE TABLE power_grid."T_substation" (
    "uuid_TP" uuid DEFAULT gen_random_uuid() NOT NULL,
    name character varying(254) NOT NULL,
    total_power_of_consumers integer NOT NULL,
    transformer_power integer NOT NULL,
    electrical_reliability integer NOT NULL,
    coordinates point NOT NULL
);


ALTER TABLE power_grid."T_substation" OWNER TO postgres;

--
-- TOC entry 202 (class 1259 OID 24811)
-- Name: Сonformity_table; Type: TABLE; Schema: power_grid; Owner: postgres
--

CREATE TABLE power_grid."Сonformity_table" (
    name_table_in_bd character varying(254) NOT NULL,
    path_to_object_in_cim character varying(254)
);


ALTER TABLE power_grid."Сonformity_table" OWNER TO postgres;

--
-- TOC entry 3032 (class 0 OID 32957)
-- Dependencies: 206
-- Data for Name: ACLineSegment; Type: TABLE DATA; Schema: power_grid; Owner: postgres
--



--
-- TOC entry 3031 (class 0 OID 32944)
-- Dependencies: 205
-- Data for Name: Cim_model_nodes; Type: TABLE DATA; Schema: power_grid; Owner: postgres
--

INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:LoadArea.Substation', 'cim:LoadArea', NULL, 'total_power_of_consumers', 3, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:EnergyConsumer.pnom', 'cim:EnergyConsumer', NULL, 'total_power_of_consumers', 4, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:PowerTransformer.MemberOf_Substation', 'cim:PowerTransformer', NULL, 'transformer_power', 5, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Location.PowerSystemResource', 'cim:Location', NULL, 'coordinates', 5, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:TransformerWinding.windingType', 'cim:TransformerWinding', NULL, 'transformer_power', 6, 'http://iec.ch/TC57/2005/CIM-schema-cim10#WindingType.primary');
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Conductor.length', NULL, NULL, 'length', 1, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:ConductorType', 'cim:ConductorType', NULL, 'section', 7, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:WireType.radius', 'cim:WireType', NULL, 'section', 4, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:PositionPoint.yPosition', 'cim:PositionPoint', NULL, 'coordinates', 1, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:PositionPoint.xPosition', 'cim:PositionPoint', NULL, 'coordinates', 1, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Naming.name', NULL, NULL, 'name', 1, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Naming.description', NULL, NULL, 'electrical_reliability', 1, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:WireArrangement.WireType', 'cim:WireArrangement', 'cim:WireType', 'section', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:WireArrangement.ConductorType', 'cim:WireArrangement', 'cim:ConductorType', 'section', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:TransformerWinding.MemberOf_PowerTransformer', 'cim:TransformerWinding', 'cim:PowerTransformer', 'transformer_power', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Location', 'cim:PositionPoint', 'cim:Location', 'coordinates', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Conductor.ConductorType', NULL, 'cim:ConductorType', 'section', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:LoadArea.EnergyConsumer', 'cim:LoadArea', 'cim:EnergyConsumer', 'total_power_of_consumers', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:ConductigEquipment.Terminals', NULL, 'cim:Terminal', 'transmitted_power', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:MeasurementValue.value', 'cim:MeasurementValue', NULL, 'transmitted_power', 4, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Measurment.Contain_MeasurementValues', 'cim:Measurment', 'cim:MeasurementValue', 'transmitted_power', 8, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Terminal.Measurments', 'cim:Terminal', 'cim:Measurment', 'transmitted_power', 8, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:TransformerWinding.ratedKV', 'cim:TransformerWinding', NULL, 'transformer_power', 2, '10');
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:TransformerWinding.ratedMVA', 'cim:TransformerWinding', NULL, 'transformer_power', 1, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Terminal.ConductingEquipment', 'cim:Terminal', NULL, 'output', 3, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Terminal.ConnectivityNode', 'cim:Terminal', 'cim:ConnectivityNode', 'output', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:ConnectivityNode.MemberOf_EquipmentContainer', 'cim:ConnectivityNode', NULL, 'output', 9, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Naming.description', 'cim:ConnectivityNode', NULL, 'output', 2, 'output');
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Naming.description', 'cim:ConnectivityNode', NULL, 'input', 2, 'input');
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Terminal.ConductingEquipment', 'cim:Terminal', NULL, 'input', 3, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:Terminal.ConnectivityNode', 'cim:Terminal', 'cim:ConnectivityNode', 'input', 0, NULL);
INSERT INTO power_grid."Cim_model_nodes" (attributes, class, link_source, name_column, resurce_or_value, const_value) VALUES ('cim:ConnectivityNode.MemberOf_EquipmentContainer', 'cim:ConnectivityNode', NULL, 'input', 9, NULL);


--
-- TOC entry 3030 (class 0 OID 32938)
-- Dependencies: 204
-- Data for Name: D_substation; Type: TABLE DATA; Schema: power_grid; Owner: postgres
--



--
-- TOC entry 3027 (class 0 OID 24757)
-- Dependencies: 201
-- Data for Name: Diesel_generator; Type: TABLE DATA; Schema: power_grid; Owner: postgres
--

INSERT INTO power_grid."Diesel_generator" ("uuid_DGU", power, coordinates, uuid_platform) VALUES ('b61278c6-2b1e-463c-a938-6e8cb695aaab', 1, '(1,1)', '448c5dd6-52cc-4cd0-9442-e67669992779');


--
-- TOC entry 3026 (class 0 OID 24751)
-- Dependencies: 200
-- Data for Name: Platform_DGU; Type: TABLE DATA; Schema: power_grid; Owner: postgres
--

INSERT INTO power_grid."Platform_DGU" (uuid_platform, coordinates) VALUES ('448c5dd6-52cc-4cd0-9442-e67669992779', '(1,1)');


--
-- TOC entry 3029 (class 0 OID 32883)
-- Dependencies: 203
-- Data for Name: T_substation; Type: TABLE DATA; Schema: power_grid; Owner: postgres
--



--
-- TOC entry 3028 (class 0 OID 24811)
-- Dependencies: 202
-- Data for Name: Сonformity_table; Type: TABLE DATA; Schema: power_grid; Owner: postgres
--

INSERT INTO power_grid."Сonformity_table" (name_table_in_bd, path_to_object_in_cim) VALUES ('T_substation', 'cim:Substation');
INSERT INTO power_grid."Сonformity_table" (name_table_in_bd, path_to_object_in_cim) VALUES ('D_substation', 'cim:Substation');
INSERT INTO power_grid."Сonformity_table" (name_table_in_bd, path_to_object_in_cim) VALUES ('ACLineSegment', 'cim:ACLineSegment');


--
-- TOC entry 2894 (class 2606 OID 32962)
-- Name: ACLineSegment ACLineSegment_pkey1; Type: CONSTRAINT; Schema: power_grid; Owner: postgres
--

ALTER TABLE ONLY power_grid."ACLineSegment"
    ADD CONSTRAINT "ACLineSegment_pkey1" PRIMARY KEY (uuid_cable);


--
-- TOC entry 2889 (class 2606 OID 32943)
-- Name: D_substation D_substation_pkey1; Type: CONSTRAINT; Schema: power_grid; Owner: postgres
--

ALTER TABLE ONLY power_grid."D_substation"
    ADD CONSTRAINT "D_substation_pkey1" PRIMARY KEY ("uuid_RP");


--
-- TOC entry 2883 (class 2606 OID 24762)
-- Name: Diesel_generator Diesel_generator_pkey; Type: CONSTRAINT; Schema: power_grid; Owner: postgres
--

ALTER TABLE ONLY power_grid."Diesel_generator"
    ADD CONSTRAINT "Diesel_generator_pkey" PRIMARY KEY ("uuid_DGU");


--
-- TOC entry 2881 (class 2606 OID 24756)
-- Name: Platform_DGU Platform_DGU_pkey; Type: CONSTRAINT; Schema: power_grid; Owner: postgres
--

ALTER TABLE ONLY power_grid."Platform_DGU"
    ADD CONSTRAINT "Platform_DGU_pkey" PRIMARY KEY (uuid_platform);


--
-- TOC entry 2887 (class 2606 OID 32888)
-- Name: T_substation T_substation_pkey1; Type: CONSTRAINT; Schema: power_grid; Owner: postgres
--

ALTER TABLE ONLY power_grid."T_substation"
    ADD CONSTRAINT "T_substation_pkey1" PRIMARY KEY ("uuid_TP");


--
-- TOC entry 2885 (class 2606 OID 24820)
-- Name: Сonformity_table correspondence_pkey; Type: CONSTRAINT; Schema: power_grid; Owner: postgres
--

ALTER TABLE ONLY power_grid."Сonformity_table"
    ADD CONSTRAINT correspondence_pkey PRIMARY KEY (name_table_in_bd);


--
-- TOC entry 2891 (class 2606 OID 33080)
-- Name: Cim_model_nodes primary_key_attributes_and_name_column; Type: CONSTRAINT; Schema: power_grid; Owner: postgres
--

ALTER TABLE ONLY power_grid."Cim_model_nodes"
    ADD CONSTRAINT primary_key_attributes_and_name_column PRIMARY KEY (attributes, name_column);


--
-- TOC entry 2892 (class 1259 OID 32963)
-- Name: ACLineSegment_output_input_key; Type: INDEX; Schema: power_grid; Owner: postgres
--

CREATE UNIQUE INDEX "ACLineSegment_output_input_key" ON power_grid."ACLineSegment" USING btree (output, input);


--
-- TOC entry 2895 (class 2606 OID 24763)
-- Name: Diesel_generator FK_Diesel_generator_Platform_DGU; Type: FK CONSTRAINT; Schema: power_grid; Owner: postgres
--

ALTER TABLE ONLY power_grid."Diesel_generator"
    ADD CONSTRAINT "FK_Diesel_generator_Platform_DGU" FOREIGN KEY (uuid_platform) REFERENCES power_grid."Platform_DGU"(uuid_platform) ON DELETE CASCADE;


-- Completed on 2021-06-06 18:21:57

--
-- PostgreSQL database dump complete
--

